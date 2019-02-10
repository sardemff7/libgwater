/*
 * libgwater-mpd - MPD GSource
 *
 * Copyright © 2014-2017 Quentin "Sardem FF7" Glidic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif /* G_LOG_DOMAIN */
#define G_LOG_DOMAIN "GWaterMpd"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#ifdef G_OS_UNIX
#include <gio/gunixsocketaddress.h>
#endif /* G_OS_UNIX */

#include <mpd/async.h>

#include "libgwater-mpd.h"

struct _GWaterMpdSource {
    GSource source;
    GSocket *socket;
    gboolean mpd_owned;
    struct mpd_async *mpd;
    enum mpd_async_event events;
    enum mpd_error error;
    gpointer fd;
};

static GIOCondition
_g_water_mpd_events_to_gio(enum mpd_async_event events)
{
    GIOCondition gio_events = 0;
    if ( events & MPD_ASYNC_EVENT_HUP )
        gio_events |= G_IO_HUP;
    if ( events & MPD_ASYNC_EVENT_ERROR )
        gio_events |= G_IO_ERR;
    if ( events & MPD_ASYNC_EVENT_READ )
        gio_events |= G_IO_IN;
    if ( events & MPD_ASYNC_EVENT_WRITE )
        gio_events |= G_IO_OUT;
    return gio_events;
}

static gboolean
_g_water_mpd_source_prepare(GSource *source, gint *timeout)
{
    GWaterMpdSource *self = (GWaterMpdSource *)source;

    enum mpd_async_event events = mpd_async_events(self->mpd);
    if ( events != self->events )
    {
        self->events = events;
        GIOCondition nevents;
        nevents = _g_water_mpd_events_to_gio(events);

        g_source_modify_unix_fd(source, self->fd, nevents);
    }

    self->error = mpd_async_get_error(self->mpd);

    *timeout = -1;
    return ( self->error != MPD_ERROR_SUCCESS );
}

static gboolean
_g_water_mpd_source_check(GSource *source)
{
    GWaterMpdSource *self = (GWaterMpdSource *)source;

    GIOCondition revents;
    revents = g_source_query_unix_fd(source, self->fd);

    return ( revents > 0 );
}

static gboolean
_g_water_mpd_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    GWaterMpdSource *self = (GWaterMpdSource *)source;

    GIOCondition revents;
    revents = g_source_query_unix_fd(source, self->fd);

    enum mpd_async_event events = 0;
    if ( revents & G_IO_HUP )
        events |= MPD_ASYNC_EVENT_HUP;
    if ( revents & G_IO_ERR )
        events |= MPD_ASYNC_EVENT_ERROR;
    if ( revents & G_IO_IN )
        events |= MPD_ASYNC_EVENT_READ;
    if ( revents & G_IO_OUT )
        events |= MPD_ASYNC_EVENT_WRITE;

    enum mpd_error error = self->error;
    self->error = MPD_ERROR_SUCCESS;

    if ( error != MPD_ERROR_SUCCESS )
        return ((GWaterMpdLineCallback)(void *)callback)(NULL, error, user_data);

    if ( events != 0 )
    {
        if ( ! mpd_async_io(self->mpd, events) )
            return ((GWaterMpdLineCallback)(void *)callback)(NULL, mpd_async_get_error(self->mpd), user_data);
    }

    gboolean ret;
    gchar *line;
    do
    {
        line = mpd_async_recv_line(self->mpd);
        error = mpd_async_get_error(self->mpd);
        if ( ( line == NULL ) && ( error == MPD_ERROR_SUCCESS ) )
            /* Wait for the end of the line */
            return G_SOURCE_CONTINUE;
    } while (( ret = ((GWaterMpdLineCallback)(void *)callback)(line, error, user_data) ));
    return ret;
}

static void
_g_water_mpd_source_finalize(GSource *source)
{
    GWaterMpdSource *self = (GWaterMpdSource *)source;

    if ( self->mpd_owned )
        mpd_async_free(self->mpd);
    if ( self->socket != NULL )
        g_object_unref(self->socket);
}

static GSourceFuncs _g_water_mpd_source_funcs = {
    .prepare  = _g_water_mpd_source_prepare,
    .check    = _g_water_mpd_source_check,
    .dispatch = _g_water_mpd_source_dispatch,
    .finalize = _g_water_mpd_source_finalize,
};

GWaterMpdSource *
g_water_mpd_source_new(GMainContext *context, const gchar *host, guint16 port, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func, GError **error)
{
    g_return_val_if_fail(host != NULL, NULL);
    g_return_val_if_fail(callback != NULL, NULL);

    GSocketAddress *address;
#ifdef G_OS_UNIX
    if ( g_path_is_absolute(host) )
        address = g_unix_socket_address_new(host);
    else
#endif /* G_OS_UNIX */
    {
        GInetAddress *inet_address;
        inet_address = g_inet_address_new_from_string(host);
        address = g_inet_socket_address_new(inet_address, port);
        g_object_unref(inet_address);
    }

    GSocket *socket;
    socket = g_socket_new(g_socket_address_get_family(address), G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, error);
    if ( socket == NULL )
    {
        g_object_unref(address);
        return NULL;
    }

    if ( ! g_socket_connect(socket, address, NULL, error) )
    {
        g_object_unref(address);
        g_object_unref(socket);
        return NULL;
    }

    GWaterMpdSource *self;

    self = g_water_mpd_source_new_for_socket(context, socket, callback, user_data, destroy_func);
    g_object_unref(socket);
    return self;
}

GWaterMpdSource *
g_water_mpd_source_new_for_socket(GMainContext *context, GSocket *socket, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(socket != NULL, NULL);
    g_return_val_if_fail(callback != NULL, NULL);

    GWaterMpdSource *self;

    self = g_water_mpd_source_new_for_fd(context, g_socket_get_fd(socket), callback, user_data, destroy_func);
    self->socket = g_object_ref(socket);
    return self;
}

GWaterMpdSource *
g_water_mpd_source_new_for_fd(GMainContext *context, gint fd, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(callback != NULL, NULL);
    g_return_val_if_fail(fd >= 0, NULL);

    struct mpd_async *mpd;
    GWaterMpdSource *self;

    mpd = mpd_async_new(fd);

    self = g_water_mpd_source_new_for_mpd(context, mpd, callback, user_data, destroy_func);
    self->mpd_owned = TRUE;
    return self;
}

GWaterMpdSource *
g_water_mpd_source_new_for_mpd(GMainContext *context, struct mpd_async *mpd, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(callback != NULL, NULL);

    GSource *source;
    GWaterMpdSource *self;

    source = g_source_new(&_g_water_mpd_source_funcs, sizeof(GWaterMpdSource));
    self = (GWaterMpdSource *)source;
    self->mpd = mpd;

    self->events = mpd_async_events(self->mpd);
    GIOCondition events;
    events = _g_water_mpd_events_to_gio(self->events);

    self->fd = g_source_add_unix_fd(source, mpd_async_get_fd(self->mpd), events);

    g_source_attach(source, context);

    g_source_set_callback(source, (GSourceFunc)(void *)callback, user_data, destroy_func);

    return self;
}

void
g_water_mpd_source_free(GWaterMpdSource *self)
{
    GSource * source = (GSource *)self;
    g_return_if_fail(self != NULL);

    g_source_destroy(source);

    g_source_unref(source);
}

struct mpd_async *
g_water_mpd_source_get_mpd(GWaterMpdSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->mpd;
}

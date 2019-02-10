/*
 * libgwater-wayland - Wayland GSource
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
#define G_LOG_DOMAIN "GWaterWaylandServer"

#include <errno.h>

#include <glib.h>

#include <wayland-server-core.h>

#include "libgwater-wayland-server.h"

struct _GWaterWaylandServerSource {
    GSource source;
    gboolean display_owned;
    struct wl_display *display;
    struct wl_event_loop *loop;
    gpointer fd;
};

static gboolean
_g_water_wayland_server_source_prepare(GSource *source, gint *timeout)
{
    GWaterWaylandServerSource *self = (GWaterWaylandServerSource *)source;

    wl_display_flush_clients(self->display);

    *timeout = -1;
    return FALSE;
}

static gboolean
_g_water_wayland_server_source_check(GSource *source)
{
    GWaterWaylandServerSource *self = (GWaterWaylandServerSource *)source;

    GIOCondition revents;
    revents = g_source_query_unix_fd(source, self->fd);

    return ( revents > 0 );
}

static gboolean
_g_water_wayland_server_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    GWaterWaylandServerSource *self = (GWaterWaylandServerSource *)source;
    GIOCondition revents;

    revents = g_source_query_unix_fd(source, self->fd);
    if ( revents & G_IO_IN )
    {
        if ( wl_event_loop_dispatch(self->loop, 0) < 0 )
        {
            if ( callback != NULL )
                return callback(user_data);
            return G_SOURCE_REMOVE;
        }
    }

    errno = 0;
    if ( revents & (G_IO_ERR | G_IO_HUP) )
    {
        if ( callback != NULL )
            return callback(user_data);
        return G_SOURCE_REMOVE;
    }

    return G_SOURCE_CONTINUE;
}

static void
_g_water_wayland_server_source_finalize(GSource *source)
{
    GWaterWaylandServerSource *self = (GWaterWaylandServerSource *)source;

    if ( self->display_owned )
        wl_display_destroy(self->display);
}

static GSourceFuncs _g_water_wayland_server_source_funcs = {
    .prepare  = _g_water_wayland_server_source_prepare,
    .check    = _g_water_wayland_server_source_check,
    .dispatch = _g_water_wayland_server_source_dispatch,
    .finalize = _g_water_wayland_server_source_finalize,
};

GWaterWaylandServerSource *
g_water_wayland_server_source_new(GMainContext *context)
{
    struct wl_display *display;
    GWaterWaylandServerSource *self;

    display = wl_display_create();
    if ( display == NULL )
        return NULL;

    self = g_water_wayland_server_source_new_for_display(context, display);
    self->display_owned = TRUE;
    return self;
}

GWaterWaylandServerSource *
g_water_wayland_server_source_new_for_display(GMainContext *context, struct wl_display *display)
{
    g_return_val_if_fail(display != NULL, NULL);

    GSource *source;
    GWaterWaylandServerSource *self;

    source = g_source_new(&_g_water_wayland_server_source_funcs, sizeof(GWaterWaylandServerSource));
    self = (GWaterWaylandServerSource *)source;
    self->display = display;
    self->loop = wl_display_get_event_loop(self->display);

    self->fd = g_source_add_unix_fd(source, wl_event_loop_get_fd(self->loop), G_IO_IN | G_IO_ERR | G_IO_HUP);

    g_source_attach(source, context);

    return self;
}

void
g_water_wayland_server_source_free(GWaterWaylandServerSource *self)
{
    GSource * source = (GSource *)self;
    g_return_if_fail(self != NULL);

    g_source_destroy(source);

    g_source_unref(source);
}

void
g_water_wayland_server_source_set_error_callback(GWaterWaylandServerSource *self, GSourceFunc callback, gpointer user_data, GDestroyNotify destroy_notify)
{
    g_return_if_fail(self != NULL);

    g_source_set_callback((GSource *)self, callback, user_data, destroy_notify);
}

struct wl_display *
g_water_wayland_server_source_get_display(GWaterWaylandServerSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->display;
}

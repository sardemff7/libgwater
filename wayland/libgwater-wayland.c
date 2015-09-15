/*
 * libgwater-wayland - Wayland GSource
 *
 * Copyright © 2012-2014 Quentin "Sardem FF7" Glidic
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
#define G_LOG_DOMAIN "GWaterWayland"

#include <errno.h>

#include <glib.h>

#include <wayland-client.h>

#include "libgwater-wayland.h"

struct _GWaterWaylandSource {
    GSource source;
    gboolean display_owned;
    struct wl_display *display;
    gpointer fd;
    int error;
};

static gboolean
_g_water_wayland_source_prepare(GSource *source, gint *timeout)
{
    GWaterWaylandSource *self = (GWaterWaylandSource *)source;

    if ( wl_display_flush(self->display) < 0 )
        self->error = errno;

    *timeout = -1;
    return FALSE;
}

static gboolean
_g_water_wayland_source_check(GSource *source)
{
    GWaterWaylandSource *self = (GWaterWaylandSource *)source;

    GIOCondition revents;
    revents = g_source_query_unix_fd(source, self->fd);

    return ( revents > 0 );
}

static gboolean
_g_water_wayland_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    GWaterWaylandSource *self = (GWaterWaylandSource *)source;

    if ( self->error > 0 )
    {
        errno = self->error;
        if ( callback != NULL )
            return callback(user_data);
        return FALSE;
    }

    GIOCondition revents;
    revents = g_source_query_unix_fd(source, self->fd);

    if ( revents & G_IO_IN )
    {
        if ( wl_display_dispatch(self->display) < 0 )
        {
            if ( callback != NULL )
                return callback(user_data);
            return FALSE;
        }
    }

    errno = 0;
    if ( revents & (G_IO_ERR | G_IO_HUP) )
    {
        if ( callback != NULL )
            return callback(user_data);
        return FALSE;
    }

    return TRUE;
}

static void
_g_water_wayland_source_finalize(GSource *source)
{
    GWaterWaylandSource *self = (GWaterWaylandSource *)source;

    if ( self->display_owned )
        wl_display_disconnect(self->display);
}

static GSourceFuncs _g_water_wayland_source_funcs = {
    _g_water_wayland_source_prepare,
    _g_water_wayland_source_check,
    _g_water_wayland_source_dispatch,
    _g_water_wayland_source_finalize
};

GWaterWaylandSource *
g_water_wayland_source_new(GMainContext *context, const gchar *name)
{
    struct wl_display *display;
    GWaterWaylandSource *self;

    display = wl_display_connect(name);
    if ( display == NULL )
        return NULL;

    self = g_water_wayland_source_new_for_display(context, display);
    self->display_owned = TRUE;
    return self;
}

GWaterWaylandSource *
g_water_wayland_source_new_for_display(GMainContext *context, struct wl_display *display)
{
    g_return_val_if_fail(display != NULL, NULL);

    GSource *source;
    GWaterWaylandSource *self;

    source = g_source_new(&_g_water_wayland_source_funcs, sizeof(GWaterWaylandSource));
    self = (GWaterWaylandSource *)source;
    self->display = display;

    self->fd = g_source_add_unix_fd(source, wl_display_get_fd(self->display), G_IO_IN | G_IO_ERR | G_IO_HUP);

    g_source_attach(source, context);

    return self;
}

void
g_water_wayland_source_ref(GWaterWaylandSource *self)
{
    g_return_if_fail(self != NULL);

    g_source_ref((GSource *)self);
}

void
g_water_wayland_source_unref(GWaterWaylandSource *self)
{
    g_return_if_fail(self != NULL);

    g_source_unref((GSource *)self);
}

void
g_water_wayland_source_set_error_callback(GWaterWaylandSource *self, GSourceFunc callback, gpointer user_data, GDestroyNotify destroy_notify)
{
    g_return_if_fail(self != NULL);

    g_source_set_callback((GSource *)self, callback, user_data, destroy_notify);
}

struct wl_display *
g_water_wayland_source_get_display(GWaterWaylandSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->display;
}

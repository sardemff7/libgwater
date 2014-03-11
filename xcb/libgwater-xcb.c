/*
 * libgwater-xcb - XCB GSource
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
#define G_LOG_DOMAIN "GWaterXcb"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#include <glib.h>

#include <xcb/xcb.h>

#include "libgwater-xcb.h"

struct _GWaterXcbSource {
    GSource source;
    gboolean connection_owned;
    xcb_connection_t *connection;
    GPollFD fd;
    GQueue *queue;
};

static void
_g_water_xcb_source_event_free(gpointer data, gpointer user_data)
{
    free(data);
}

static gboolean
_g_water_xcb_source_prepare(GSource *source, gint *timeout)
{
    GWaterXcbSource *self = (GWaterXcbSource *)source;
    xcb_flush(self->connection);
    *timeout = -1;
    return ! g_queue_is_empty(self->queue);
}

static gboolean
_g_water_xcb_source_check(GSource *source)
{
    GWaterXcbSource *self = (GWaterXcbSource *)source;

    if ( self->fd.revents & G_IO_IN )
    {
        xcb_generic_event_t *event;

        if ( xcb_connection_has_error(self->connection) )
            return TRUE;

        while ( ( event = xcb_poll_for_event(self->connection) ) != NULL )
            g_queue_push_tail(self->queue, event);
    }

    return ! g_queue_is_empty(self->queue);
}

static gboolean
_g_water_xcb_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    GWaterXcbSource *self = (GWaterXcbSource *)source;
    xcb_generic_event_t *event;

    gboolean ret;

    event = g_queue_pop_head(self->queue);
    ret = ((GWaterXcbEventCallback)callback)(event, user_data);
    _g_water_xcb_source_event_free(event, NULL);

    return ret;
}

static void
_g_water_xcb_source_finalize(GSource *source)
{
    GWaterXcbSource *self = (GWaterXcbSource *)source;

    g_queue_foreach(self->queue, _g_water_xcb_source_event_free, NULL);
    g_queue_free(self->queue);

    if ( self->connection_owned )
        xcb_disconnect(self->connection);
}

static GSourceFuncs _g_water_xcb_source_funcs = {
    _g_water_xcb_source_prepare,
    _g_water_xcb_source_check,
    _g_water_xcb_source_dispatch,
    _g_water_xcb_source_finalize
};

GWaterXcbSource *
g_water_xcb_source_new(GMainContext *context, const gchar *display, gint *screen, GWaterXcbEventCallback callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(callback != NULL, NULL);

    xcb_connection_t *connection;
    GWaterXcbSource *source;

    connection = xcb_connect(display, screen);
    if ( xcb_connection_has_error(connection) )
    {
        xcb_disconnect(connection);
        return NULL;
    }

    source = g_water_xcb_source_new_for_connection(context, connection, callback, user_data, destroy_func);
    source->connection_owned = TRUE;
    return source;
}

GWaterXcbSource *
g_water_xcb_source_new_for_connection(GMainContext *context, xcb_connection_t *connection, GWaterXcbEventCallback callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(callback != NULL, NULL);

    GWaterXcbSource *source;

    source = (GWaterXcbSource *)g_source_new(&_g_water_xcb_source_funcs, sizeof(GWaterXcbSource));

    source->connection = connection;
    source->queue = g_queue_new();

    source->fd.fd = xcb_get_file_descriptor(connection);
    source->fd.events = G_IO_IN;

    g_source_add_poll((GSource *)source, &source->fd);
    g_source_attach((GSource *)source, context);

    g_source_set_callback((GSource *)source, (GSourceFunc)callback, user_data, destroy_func);

    return source;
}

void
g_water_xcb_source_ref(GWaterXcbSource *self)
{
    g_return_if_fail(self != NULL);

    g_source_ref((GSource *)self);
}

void
g_water_xcb_source_unref(GWaterXcbSource *self)
{
    g_return_if_fail(self != NULL);

    g_source_unref((GSource *)self);
}

xcb_connection_t *
g_water_xcb_source_get_connection(GWaterXcbSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->connection;
}

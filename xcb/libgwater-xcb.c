/*
 * libgwater-xcb - XCB GSource
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
#define G_LOG_DOMAIN "GWaterXcb"

#include <stdlib.h>

#include <glib.h>

#include <xcb-imdkit/encoding.h>
#include <xcb-imdkit/imclient.h>
#include <xcb-imdkit/ximproto.h>
#include <xcb/xcb.h>

#include "libgwater-xcb.h"

struct _GWaterXcbSource {
    GSource source;
    gboolean connection_owned;
    xcb_connection_t *connection;
    xcb_xim_t *im;
    gpointer fd;
    GQueue *queue;
};

static void
_g_water_xcb_source_event_free(gpointer data)
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

    GIOCondition revents;
    revents = g_source_query_unix_fd(source, self->fd);

    if ( revents & G_IO_IN )
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
    ret = ((GWaterXcbEventCallback)(void *)callback)(event, user_data);
    _g_water_xcb_source_event_free(event);

    return ret;
}

static void
_g_water_xcb_source_finalize(GSource *source)
{
    GWaterXcbSource *self = (GWaterXcbSource *)source;

    g_queue_free_full(self->queue, _g_water_xcb_source_event_free);

    if ( self->connection_owned ) {
        xcb_xim_close(self->im);
        xcb_xim_destroy(self->im);
        xcb_disconnect(self->connection);
    }
}

static GSourceFuncs _g_water_xcb_source_funcs = {
    .prepare  = _g_water_xcb_source_prepare,
    .check    = _g_water_xcb_source_check,
    .dispatch = _g_water_xcb_source_dispatch,
    .finalize = _g_water_xcb_source_finalize,
};

GWaterXcbSource *
g_water_xcb_source_new(GMainContext *context, const gchar *display, gint *screen, GWaterXcbEventCallback callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(callback != NULL, NULL);

    xcb_connection_t *connection;
    GWaterXcbSource *self;
    xcb_compound_text_init();

    connection = xcb_connect(display, screen);
    if ( xcb_connection_has_error(connection) )
    {
        xcb_disconnect(connection);
        return NULL;
    }

    xcb_xim_t *im = xcb_xim_create(connection, *screen, NULL);
    self = g_water_xcb_source_new_for_connection(context, connection, im, callback, user_data, destroy_func);
    self->connection_owned = TRUE;
    return self;
}

GWaterXcbSource *
g_water_xcb_source_new_for_connection(GMainContext *context, xcb_connection_t *connection,
				      xcb_xim_t *im, GWaterXcbEventCallback callback, 
				      gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(connection != NULL, NULL);
    g_return_val_if_fail(callback != NULL, NULL);

    GSource *source;
    GWaterXcbSource *self;

    source = g_source_new(&_g_water_xcb_source_funcs, sizeof(GWaterXcbSource));
    self = (GWaterXcbSource *)source;
    self->connection = connection;
    self->im = im;

    self->queue = g_queue_new();

    self->fd = g_source_add_unix_fd(source, xcb_get_file_descriptor(self->connection), G_IO_IN);

    g_source_attach(source, context);

    g_source_set_callback(source, (GSourceFunc)(void *)callback, user_data, destroy_func);

    xcb_xim_set_use_compound_text(self->im, true);
    xcb_xim_set_use_utf8_string(self->im, true);

    return self;
}

void
g_water_xcb_source_free(GWaterXcbSource *self)
{
    GSource * source = (GSource *)self;
    g_return_if_fail(self != NULL);

    g_source_destroy(source);

    g_source_unref(source);
}

xcb_connection_t *
g_water_xcb_source_get_connection(GWaterXcbSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->connection;
}

xcb_xim_t *
g_water_xcb_source_get_im(GWaterXcbSource *self) {
    g_return_val_if_fail(self != NULL, NULL);

    return self->im;
}

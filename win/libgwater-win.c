/*
 * libgwater-win - Windows GSource
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
#define G_LOG_DOMAIN "GWaterWin"

#include <glib.h>

#include <Windows.h>

#include "libgwater-win.h"

struct _GWaterWinSource {
    GSource source;
    GPollFD fd;
    guint events;
};

static gboolean
_g_water_win_source_prepare(GSource *source, gint *timeout)
{
    GWaterWinSource *self = (GWaterWinSource *)source;
    *timeout = -1;
    return GetQueueStatus(self->events);
}

static gboolean
_g_water_win_source_check(GSource *source)
{
    GWaterWinSource *self = (GWaterWinSource *)source;

    gboolean ret = FALSE;
    if ( self->fd.revents & G_IO_IN )
    {
        MSG msg;
        ret = PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
    }

    return ret;
}

static gboolean
_g_water_win_source_dispatch(G_GNUC_UNUSED GSource *source, G_GNUC_UNUSED GSourceFunc callback, G_GNUC_UNUSED gpointer user_data)
{
    MSG msg;
    while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return G_SOURCE_CONTINUE;
}

static void
_g_water_win_source_finalize(GSource *source)
{
}

static GSourceFuncs _g_water_win_source_funcs = {
    _g_water_win_source_prepare,
    _g_water_win_source_check,
    _g_water_win_source_dispatch,
    _g_water_win_source_finalize
};

GWaterWinSource *
g_water_win_source_new(GMainContext *context, guint events)
{
    GSource *source;
    GWaterWinSource *self;

    source = g_source_new(&_g_water_win_source_funcs, sizeof(GWaterWinSource));
    self = (GWaterWinSource *)source;
    self->events = events;

    self->fd.fd = G_WIN32_MSG_HANDLE;
    self->fd.events = G_IO_IN|G_IO_HUP;
    g_source_add_poll(source, &self->fd);

    g_source_attach(source, context);

    return self;
}

void
g_water_win_source_free(GWaterWinSource *self)
{
    GSource * source = (GSource *)self;
    g_return_if_fail(self != NULL);

    g_source_destroy(source);

    g_source_unref(source);
}

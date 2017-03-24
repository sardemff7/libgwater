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

#ifndef __G_WATER_MPD_H__
#define __G_WATER_MPD_H__

G_BEGIN_DECLS

#include <gio/gio.h>

typedef struct _GWaterMpdSource GWaterMpdSource;

typedef gboolean (*GWaterMpdLineCallback)(gchar *line, enum mpd_error error, gpointer user_data);

GWaterMpdSource *g_water_mpd_source_new(GMainContext *context, const gchar *host, guint16 port, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func, GError **error);
GWaterMpdSource *g_water_mpd_source_new_for_socket(GMainContext *context, GSocket *socket, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func);
GWaterMpdSource *g_water_mpd_source_new_for_fd(GMainContext *context, gint fd, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func);
GWaterMpdSource *g_water_mpd_source_new_for_mpd(GMainContext *context, struct mpd_async *mpd, GWaterMpdLineCallback callback, gpointer user_data, GDestroyNotify destroy_func);
void g_water_mpd_source_free(GWaterMpdSource *self);

struct mpd_async *g_water_mpd_source_get_mpd(GWaterMpdSource *source);

G_END_DECLS

#endif /* __G_WATER_MPD_H__ */

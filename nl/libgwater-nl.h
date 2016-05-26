/*
 * libgwater-nl - Netlink GSource
 *
 * Copyright © 2014-2016 Quentin "Sardem FF7" Glidic
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

#ifndef __G_WATER_NL_H__
#define __G_WATER_NL_H__

G_BEGIN_DECLS

typedef struct _GWaterNlSource GWaterNlSource;

GWaterNlSource *g_water_nl_source_new_cache_mngr(GMainContext *context, gint protocol, gint flags, gint *error);
GWaterNlSource *g_water_nl_source_new_sock(GMainContext *context, gint protocol);
GWaterNlSource *g_water_nl_source_new_for_cache_mngr(GMainContext *context, struct nl_cache_mngr *cache_mngr);
GWaterNlSource *g_water_nl_source_new_for_sock(GMainContext *context, struct nl_sock *sock);
void g_water_nl_source_free(GWaterNlSource *self);

struct nl_sock *g_water_nl_source_get_sock(GWaterNlSource *source);
struct nl_cache_mngr *g_water_nl_source_get_cache_mngr(GWaterNlSource *source);

G_END_DECLS

#endif /* __G_WATER_NL_H__ */

/*
 * libgwater-nl - Netlink GSource
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
#define G_LOG_DOMAIN "GWaterNl"

#include <glib.h>

#include <linux/netlink.h>
#include <netlink/socket.h>
#include <netlink/cache.h>
#include <netlink/msg.h>

#include "libgwater-nl.h"

struct _GWaterNlSource {
    GSource source;
    gboolean owned;
    struct nl_sock *sock;
    struct nl_cache_mngr *cache_mngr;
    gpointer fd;
};

static gboolean
_g_water_nl_source_prepare(G_GNUC_UNUSED GSource *source, gint *timeout)
{
    *timeout = -1;
    return FALSE;
}

static gboolean
_g_water_nl_source_check(GSource *source)
{
    GWaterNlSource *self = (GWaterNlSource *)source;

    return ( g_source_query_unix_fd(source, self->fd) > 0 );
}

static gboolean
_g_water_nl_source_dispatch(GSource *source, G_GNUC_UNUSED GSourceFunc callback_, G_GNUC_UNUSED gpointer user_data)
{
    GWaterNlSource *self = (GWaterNlSource *)source;

    if ( self->cache_mngr != NULL )
        nl_cache_mngr_data_ready(self->cache_mngr);
    else if ( self->sock != NULL )
        nl_recvmsgs_default(self->sock);
    else
        g_assert_not_reached();

    return G_SOURCE_CONTINUE;
}

static void
_g_water_nl_source_finalize(GSource *source)
{
    GWaterNlSource *self = (GWaterNlSource *)source;

    if ( self->owned )
    {
        nl_socket_free(self->sock);
        nl_cache_mngr_free(self->cache_mngr);
    }
}

static GSourceFuncs _g_water_nl_source_funcs = {
    .prepare  = _g_water_nl_source_prepare,
    .check    = _g_water_nl_source_check,
    .dispatch = _g_water_nl_source_dispatch,
    .finalize = _g_water_nl_source_finalize
};

GWaterNlSource *
g_water_nl_source_new_cache_mngr(GMainContext *context, gint protocol, gint flags, gint *error)
{
    g_return_val_if_fail(error == NULL || *error == 0, NULL);

    struct nl_sock *sock;
    struct nl_cache_mngr *cache_mngr;
    gint _inner_error_;
    GWaterNlSource *self;

    sock = nl_socket_alloc();

    if ( sock == NULL )
        return NULL;

    _inner_error_ = nl_cache_mngr_alloc(sock, protocol, flags, &cache_mngr);
    if ( _inner_error_ < 0 )
    {
        if ( error != NULL )
            *error = _inner_error_;
        return NULL;
    }

    self = g_water_nl_source_new_for_cache_mngr(context, cache_mngr);
    self->sock = sock;
    self->owned = TRUE;

    return self;
}

GWaterNlSource *
g_water_nl_source_new_sock(GMainContext *context, gint protocol)
{
    struct nl_sock *sock;
    GWaterNlSource *self;

    sock = nl_socket_alloc();

    if ( sock == NULL )
        return NULL;

    if ( nl_connect(sock, protocol) < 0 )
    {
        nl_socket_free(sock);
        return NULL;
    }

    self = g_water_nl_source_new_for_sock(context, sock);
    self->owned = TRUE;

    return self;
}

GWaterNlSource *
g_water_nl_source_new_for_cache_mngr(GMainContext *context, struct nl_cache_mngr *cache_mngr)
{
    g_return_val_if_fail(cache_mngr != NULL, NULL);

    GSource *source;
    GWaterNlSource *self;

    source = g_source_new(&_g_water_nl_source_funcs, sizeof(GWaterNlSource));
    self = (GWaterNlSource *)source;
    self->cache_mngr = cache_mngr;

    self->fd = g_source_add_unix_fd(source, nl_cache_mngr_get_fd(self->cache_mngr), G_IO_IN | G_IO_ERR | G_IO_HUP);

    g_source_attach(source, context);

    return self;
}

GWaterNlSource *
g_water_nl_source_new_for_sock(GMainContext *context, struct nl_sock *sock)
{
    g_return_val_if_fail(sock != NULL, NULL);

    GSource *source;
    GWaterNlSource *self;

    source = g_source_new(&_g_water_nl_source_funcs, sizeof(GWaterNlSource));
    self = (GWaterNlSource *)source;
    self->sock = sock;

    nl_socket_set_nonblocking(self->sock);

    self->fd = g_source_add_unix_fd(source, nl_socket_get_fd(self->sock), G_IO_IN | G_IO_ERR | G_IO_HUP);

    g_source_attach(source, context);

    return self;
}

void
g_water_nl_source_free(GWaterNlSource *self)
{
    GSource * source = (GSource *)self;
    g_return_if_fail(self != NULL);

    g_source_destroy(source);

    g_source_unref(source);
}

struct nl_sock *
g_water_nl_source_get_sock(GWaterNlSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->sock;
}

struct nl_cache_mngr *
g_water_nl_source_get_cache_mngr(GWaterNlSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->cache_mngr;
}

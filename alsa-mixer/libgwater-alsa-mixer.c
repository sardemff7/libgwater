/*
 * libgwater-alsa-mixer - ALSA Mixer GSource
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
#define G_LOG_DOMAIN "GWaterAlsaMixer"

#include <sys/poll.h>

#include <glib.h>

#include <asoundlib.h>

#include "libgwater-alsa-mixer.h"

struct _GWaterAlsaMixerSource {
    GSource source;
    gboolean mixer_owned;
    snd_mixer_t *mixer;
    gint size;
    gpointer *fds;
};

static gboolean
_g_water_alsa_mixer_source_prepare(GSource *source, gint *timeout)
{
    *timeout = -1;
    return FALSE;
}

static gboolean
_g_water_alsa_mixer_source_check(GSource *source)
{
    GWaterAlsaMixerSource *self = (GWaterAlsaMixerSource *)source;

    GIOCondition revents = 0;
    gint i;
    for ( i = 0 ; i < self->size ; ++i )
        revents |= g_source_query_unix_fd(source, self->fds[i]);

    return ( revents > 0 );
}

static gboolean
_g_water_alsa_mixer_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    GWaterAlsaMixerSource *self = (GWaterAlsaMixerSource *)source;

    snd_mixer_handle_events(self->mixer);

    return TRUE;
}

static void
_g_water_alsa_mixer_source_finalize(GSource *source)
{
    GWaterAlsaMixerSource *self = (GWaterAlsaMixerSource *)source;

    g_free(self->fds);

    if ( self->mixer_owned )
        snd_mixer_close(self->mixer);
}

static GSourceFuncs _g_water_alsa_mixer_source_funcs = {
    _g_water_alsa_mixer_source_prepare,
    _g_water_alsa_mixer_source_check,
    _g_water_alsa_mixer_source_dispatch,
    _g_water_alsa_mixer_source_finalize
};

GWaterAlsaMixerSource *
g_water_alsa_mixer_source_new(GMainContext *context, const gchar *card, snd_mixer_callback_t callback, gpointer user_data, GDestroyNotify destroy_func, gint *error)
{
    g_return_val_if_fail(callback != NULL, NULL);

    gint _inner_error_;
    snd_mixer_t *mixer;
    GWaterAlsaMixerSource *self;

    _inner_error_ = snd_mixer_open(&mixer, 0);
    if ( _inner_error_ < 0 )
    {
        if ( error != NULL )
            *error = _inner_error_;
        return NULL;
    }

    _inner_error_ = snd_mixer_attach(mixer, card);
    if ( _inner_error_ < 0 )
    {
        if ( error != NULL )
            *error = _inner_error_;
        return NULL;
    }

    if ( error != NULL )
        *error = 0;

    self = g_water_alsa_mixer_source_new_for_mixer(context, mixer, callback, user_data, destroy_func);
    self->mixer_owned = TRUE;
    return self;
}

GWaterAlsaMixerSource *
g_water_alsa_mixer_source_new_for_mixer(GMainContext *context, snd_mixer_t *mixer, snd_mixer_callback_t callback, gpointer user_data, GDestroyNotify destroy_func)
{
    g_return_val_if_fail(mixer != NULL, NULL);
    g_return_val_if_fail(callback != NULL, NULL);

    GSource *source;
    GWaterAlsaMixerSource *self;

    source = g_source_new(&_g_water_alsa_mixer_source_funcs, sizeof(GWaterAlsaMixerSource));
    self = (GWaterAlsaMixerSource *)source;
    self->mixer = mixer;

    gint i;
    self->size = snd_mixer_poll_descriptors_count(self->mixer);
    struct pollfd fds[self->size];
    snd_mixer_poll_descriptors(self->mixer, fds, self->size);

    self->fds = g_new(gpointer, self->size);
    for ( i = 0 ; i < self->size ; ++i )
        self->fds[i] = g_source_add_unix_fd(source, fds[i].fd, G_IO_IN | G_IO_ERR | G_IO_HUP);

    g_source_attach(source, context);

    snd_mixer_set_callback(self->mixer, callback);
    snd_mixer_set_callback_private(self->mixer, user_data);

    return self;
}

void
g_water_alsa_mixer_source_ref(GWaterAlsaMixerSource *self)
{
    g_return_if_fail(self != NULL);

    g_source_ref((GSource *)self);
}

void
g_water_alsa_mixer_source_unref(GWaterAlsaMixerSource *self)
{
    g_return_if_fail(self != NULL);

    g_source_unref((GSource *)self);
}

snd_mixer_t *
g_water_alsa_mixer_source_get_mixer(GWaterAlsaMixerSource *self)
{
    g_return_val_if_fail(self != NULL, NULL);

    return self->mixer;
}

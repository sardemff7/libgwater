/*
 * libgwater-alsa-mixer - ALSA Mixer GSource
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

#ifndef __G_WATER_ALSA_MIXER_H__
#define __G_WATER_ALSA_MIXER_H__

G_BEGIN_DECLS

typedef struct _GWaterAlsaMixerSource GWaterAlsaMixerSource;

GWaterAlsaMixerSource *g_water_alsa_mixer_source_new(GMainContext *context, const gchar *card, snd_mixer_callback_t callback, gpointer user_data, GDestroyNotify destroy_func, gint *error);
GWaterAlsaMixerSource *g_water_alsa_mixer_source_new_for_mixer(GMainContext *context, snd_mixer_t *mixer, snd_mixer_callback_t callback, gpointer user_data, GDestroyNotify destroy_func);
void g_water_alsa_mixer_source_free(GWaterAlsaMixerSource *self);

snd_mixer_t *g_water_alsa_mixer_source_get_mixer(GWaterAlsaMixerSource *source);

G_END_DECLS

#endif /* __G_WATER_ALSA_MIXER_H__ */

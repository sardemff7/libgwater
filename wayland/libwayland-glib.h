/*
 * libwayland-glib - Library to integrate Wayland nicely with GLib
 *
 * Copyright © 2012 Quentin "Sardem FF7" Glidic
 *
 * This file is part of libwayland-glib.
 *
 * libwayland-glib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libwayland-glib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libwayland-glib. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __LIBWAYLAND_GLIB_H__
#define __LIBWAYLAND_GLIB_H__

G_BEGIN_DECLS

typedef struct _GWaylandSource GWaylandSource;

GWaylandSource *g_wayland_source_new(GMainContext *context, const gchar *name);
GWaylandSource *g_wayland_source_new_for_display(GMainContext *context, struct wl_display *display);
void g_wayland_source_ref(GWaylandSource *self);
void g_wayland_source_unref(GWaylandSource *self);

void g_wayland_source_set_error_callback(GWaylandSource *self, GSourceFunc callback, gpointer user_data, GDestroyNotify destroy_notify);
struct wl_display *g_wayland_source_get_display(GWaylandSource *source);

G_END_DECLS

#endif /* __LIBWAYLAND_GLIB_H__ */

noinst_LTLIBRARIES += \
	%D%/libgwater-wayland-server.la

%C%_libgwater_wayland_server_la_SOURCES = \
	%D%/wayland-server/libgwater-wayland-server.c \
	%D%/wayland-server/libgwater-wayland-server.h

%C%_libgwater_wayland_server_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_WAYLAND_SERVER_INTERNAL_CFLAGS)


%C%_libgwater_wayland_server_la_LIBADD = \
	$(GW_WAYLAND_SERVER_INTERNAL_LIBS)

GW_WAYLAND_SERVER_CFLAGS = \
	-I$(srcdir)/%D%/wayland-server \
	$(GW_WAYLAND_SERVER_INTERNAL_CFLAGS)

GW_WAYLAND_SERVER_LIBS = \
	%D%/libgwater-wayland-server.la \
	$(GW_WAYLAND_SERVER_INTERNAL_LIBS)


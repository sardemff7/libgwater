noinst_LTLIBRARIES += \
	%D%/libgwater-wayland.la

%C%_libgwater_wayland_la_SOURCES = \
	%D%/wayland/libgwater-wayland.c \
	%D%/wayland/libgwater-wayland.h

%C%_libgwater_wayland_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_WAYLAND_INTERNAL_CFLAGS)


%C%_libgwater_wayland_la_LIBADD = \
	$(GW_WAYLAND_INTERNAL_LIBS)

GW_WAYLAND_CFLAGS = \
	-I$(srcdir)/%D%/wayland \
	$(GW_WAYLAND_INTERNAL_CFLAGS)

GW_WAYLAND_LIBS = \
	%D%/libgwater-wayland.la \
	$(GW_WAYLAND_INTERNAL_LIBS)


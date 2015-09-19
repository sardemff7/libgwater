noinst_@LIBRARY_VARIABLE@LIBRARIES += \
	@gw_dir@/libgwater-@component@.@library_suffix@a

@gw_dir_canon@_libgwater_@component_canon@_@library_suffix@a_SOURCES = \
	@gw_dir@/@component@/libgwater-@component@.c \
	@gw_dir@/@component@/libgwater-@component@.h

@gw_dir_canon@_libgwater_@component_canon@_@library_suffix@a_CFLAGS = \
	$(AM_CFLAGS) \
	$(@PREFIX@_INTERNAL_CFLAGS)

@PREFIX@_CFLAGS = \
	-I$(srcdir)/@gw_dir@/@component@ \
	$(@PREFIX@_INTERNAL_CFLAGS)

@PREFIX@_LIBS = \
	@gw_dir@/libgwater-@component@.@library_suffix@a \
	$(@PREFIX@_INTERNAL_LIBS)


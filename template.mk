noinst_@LIBRARY_VARIABLE@LIBRARIES += \
	@gw_dir@/libgwater-@component@.@library_suffix@a

@gw_dir_canon@_libgwater_@component_canon@_@library_suffix@a_SOURCES = \
	@gw_dir@/@component@/libgwater-@component@.c \
	@gw_dir@/@component@/libgwater-@component@.h \
	@config_h@

@gw_dir_canon@_libgwater_@component_canon@_@library_suffix@a_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(@PREFIX@_CPPFLAGS)

@gw_dir_canon@_libgwater_@component_canon@_@library_suffix@a_CFLAGS = \
	$(AM_CFLAGS) \
	$(@PREFIX@_CFLAGS)

@PREFIX@_CPPFLAGS += \
	-I$(srcdir)/@gw_dir@/@component@

@PREFIX@_LIBS += \
	@gw_dir@/libgwater-@component@.@library_suffix@a


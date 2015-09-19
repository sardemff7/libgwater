noinst_LTLIBRARIES += \
	%D%/libgwater-xcb.la

%C%_libgwater_xcb_la_SOURCES = \
	%D%/xcb/libgwater-xcb.c \
	%D%/xcb/libgwater-xcb.h

%C%_libgwater_xcb_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_XCB_INTERNAL_CFLAGS)


%C%_libgwater_xcb_la_LIBADD = \
	$(GW_XCB_INTERNAL_LIBS)

GW_XCB_CFLAGS = \
	-I$(srcdir)/%D%/xcb \
	$(GW_XCB_INTERNAL_CFLAGS)

GW_XCB_LIBS = \
	%D%/libgwater-xcb.la \
	$(GW_XCB_INTERNAL_LIBS)


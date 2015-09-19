noinst_LTLIBRARIES += \
	%D%/libgwater-nl.la

%C%_libgwater_nl_la_SOURCES = \
	%D%/nl/libgwater-nl.c \
	%D%/nl/libgwater-nl.h

%C%_libgwater_nl_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_NL_INTERNAL_CFLAGS)


%C%_libgwater_nl_la_LIBADD = \
	$(GW_NL_INTERNAL_LIBS)

GW_NL_CFLAGS = \
	-I$(srcdir)/%D%/nl \
	$(GW_NL_INTERNAL_CFLAGS)

GW_NL_LIBS = \
	%D%/libgwater-nl.la \
	$(GW_NL_INTERNAL_LIBS)


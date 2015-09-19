noinst_LTLIBRARIES += \
	%D%/libgwater-mpd.la

%C%_libgwater_mpd_la_SOURCES = \
	%D%/mpd/libgwater-mpd.c \
	%D%/mpd/libgwater-mpd.h

%C%_libgwater_mpd_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_MPD_INTERNAL_CFLAGS)


%C%_libgwater_mpd_la_LIBADD = \
	$(GW_MPD_INTERNAL_LIBS)

GW_MPD_CFLAGS = \
	-I$(srcdir)/%D%/mpd \
	$(GW_MPD_INTERNAL_CFLAGS)

GW_MPD_LIBS = \
	%D%/libgwater-mpd.la \
	$(GW_MPD_INTERNAL_LIBS)


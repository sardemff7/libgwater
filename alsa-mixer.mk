noinst_LTLIBRARIES += \
	%D%/libgwater-alsa-mixer.la

%C%_libgwater_alsa_mixer_la_SOURCES = \
	%D%/alsa-mixer/libgwater-alsa-mixer.c \
	%D%/alsa-mixer/libgwater-alsa-mixer.h

%C%_libgwater_alsa_mixer_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_ALSA_MIXER_INTERNAL_CFLAGS)


%C%_libgwater_alsa_mixer_la_LIBADD = \
	$(GW_ALSA_MIXER_INTERNAL_LIBS)

GW_ALSA_MIXER_CFLAGS = \
	-I$(srcdir)/%D%/alsa-mixer \
	$(GW_ALSA_MIXER_INTERNAL_CFLAGS)

GW_ALSA_MIXER_LIBS = \
	%D%/libgwater-alsa-mixer.la \
	$(GW_ALSA_MIXER_INTERNAL_LIBS)


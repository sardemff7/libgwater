noinst_LTLIBRARIES += \
	%D%/libgwater-win.la

%C%_libgwater_win_la_SOURCES = \
	%D%/win/libgwater-win.c \
	%D%/win/libgwater-win.h

%C%_libgwater_win_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GW_WIN_INTERNAL_CFLAGS)


%C%_libgwater_win_la_LIBADD = \
	$(GW_WIN_INTERNAL_LIBS)

GW_WIN_CFLAGS = \
	-I$(srcdir)/%D%/win \
	$(GW_WIN_INTERNAL_CFLAGS)

GW_WIN_LIBS = \
	%D%/libgwater-win.la \
	$(GW_WIN_INTERNAL_LIBS)


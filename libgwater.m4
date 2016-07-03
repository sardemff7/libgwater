AC_DEFUN([GW_INIT], [
    AC_REQUIRE([PKG_PROG_PKG_CONFIG])

    gw_glib_min_version="2.36"

    m4_define([GW_INIT])
])

# $1 component
# $2 canonalized component
# $3 [user optional] prefix
# $4 [optional] pkg-config packages
# $5 [optional] headers
AC_DEFUN([_GW_CHECK_INTERNAL], [
    PKG_CHECK_MODULES([$3][_INTERNAL], [glib-2.0 >= ${gw_glib_min_version} $4])

    m4_ifnblank([$5], [
        [$2]_missing_headers=""
        AC_CHECK_HEADERS([$5], [], [], [m4_foreach_w([_gw_header], [$5], [
            [#]ifdef AS_TR_CPP([HAVE_]_gw_header)
            [#]include <_gw_header>
            [#]endif
        ])])
        m4_foreach_w([_gw_header], [$5], [
            AS_IF([test x${]AS_TR_SH([ac_cv_header_]_gw_header)[} != xyes], [[$2]_missing_headers="${[$2]_missing_headers} ]_gw_header["])
        ])
        AS_IF([test -n "${[$2]_missing_headers}"], [
            AC_MSG_ERROR([Missing headers for libgwater-[$1]: ${[$2]_missing_headers}])
        ])
    ])
])

# $1 component
# $2 [optional] pkg-config packages
# $3 [optional] headers
# $4 [user optional] prefix
AC_DEFUN([_GW_CHECK], [
    AC_REQUIRE([GW_INIT])
    _GW_CHECK_INTERNAL([$1], AS_TR_SH([gw_][$1]), AS_TR_CPP([GW_][$1]), [$2], [$3])
])

AC_DEFUN([GW_CHECK_WAYLAND], [
    gw_wayland_wayland_min_version="1.1.91"

    _GW_CHECK([wayland], [wayland-client >= ${gw_wayland_wayland_min_version} $1], [errno.h $2])
])

AC_DEFUN([GW_CHECK_WAYLAND_SERVER], [
    gw_wayland_wayland_min_version="1.1.91"

    _GW_CHECK([wayland-server], [wayland-server >= ${gw_wayland_wayland_min_version} $1], [errno.h $2])
])

AC_DEFUN([GW_CHECK_XCB], [
    _GW_CHECK([xcb], [xcb $1], [stdlib.h $2])
])

AC_DEFUN([GW_CHECK_MPD], [
    gw_mpd_gio_unix=
    PKG_CHECK_EXISTS([gio-unix-2.0], [gw_mpd_have_gio_unix=yes], [gw_mpd_have_gio_unix=no])
    AS_IF([test x${gw_mpd_have_gio_unix} = xyes], [
        gw_mpd_gio_unix="gio-unix-2.0"
    ])

    _GW_CHECK([mpd], [libmpdclient gobject-2.0 gio-2.0 ${gw_mpd_gio_unix} $1], [errno.h sys/types.h sys/socket.h $2])
])

AC_DEFUN([GW_CHECK_ALSA_MIXER], [
    _GW_CHECK([alsa-mixer], [alsa $1], [sys/poll.h $2])
])

AC_DEFUN([GW_CHECK_NL], [
    _GW_CHECK([nl], [libnl-3.0 $1], [linux/netlink.h $2])
])

AC_DEFUN([GW_CHECK_WIN], [
    _GW_CHECK([win], [$1], [Windows.h $2])
])

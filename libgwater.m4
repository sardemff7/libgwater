AC_DEFUN([GW_INIT], [
    AC_REQUIRE([PKG_PROG_PKG_CONFIG])

    m4_define([_gw_dir], m4_default([$1], [libgwater]))
    m4_define([_gw_dir_canon], m4_translit(_gw_dir, [/+-], [___]))

    m4_ifdef([LT_INIT],[
        m4_define([_gw_library_suffix], [l])
        m4_define([_GW_LIBRARY_VARIABLE], [LT])
    ],[
        m4_define([_gw_library_suffix], [])
        m4_define([_GW_LIBRARY_VARIABLE], [])
    ])

    gw_glib_min_version="2.36"

    m4_define([GW_INIT])
])

# $1 component
# $2 canonalized component
# $3 [user optional] prefix
# $4 [optional] pkg-config packages
# $5 [optional] headers
AC_DEFUN([_GW_CHECK_INTERNAL], [
    m4_syscmd([sed]dnl
        [-e 's:@gw_dir@:]_gw_dir[:g']dnl
        [-e 's:@gw_dir_canon@:]_gw_dir_canon[:g']dnl
        [-e 's:@library_suffix@:]_gw_library_suffix[:g']dnl
        [-e 's:@LIBRARY_VARIABLE@:]_GW_LIBRARY_VARIABLE[:g']dnl
        [-e 's:@component@:$1:g']dnl
        [-e 's:@component_canon@:$2:g']dnl
        [-e 's:@PREFIX@:$3:g']dnl
        _gw_dir[/template.mk > ]_gw_dir[/][$1][.mk]
    )

    PKG_CHECK_MODULES([$3], [glib-2.0 >= ${gw_glib_min_version} $4])

    m4_ifnblank([$5], [
        gw_][$2][_missing_headers=""
        AC_CHECK_HEADERS([$5], [], [], [m4_foreach_w([_gw_header], [$5], [
            [#]ifdef AS_TR_CPP([HAVE_]_gw_header)
            [#]include <_gw_header>
            [#]endif
        ])])
        m4_foreach_w([_gw_header], [$5], [
            AS_IF([test x${ac_cv_header_]m4_translit(_gw_header, [-/.], [___])[} != xyes], [gw_][$2][_missing_headers="${gw_][$2][_missing_headers} ]_gw_header["])
        ])
        AS_IF([test -n "${gw_][$2][_missing_headers}"], [
            AC_MSG_ERROR([Missing headers for libgwater-][$2][ : ${gw_][$3][_missing_headers}])
        ])
    ])
])

# $1 component
# $2 [optional] pkg-config packages
# $3 [optional] headers
# $4 [user optional] prefix
AC_DEFUN([_GW_CHECK], [
    AC_REQUIRE([GW_INIT])
    _GW_CHECK_INTERNAL([$1], m4_translit([$1], [-], [_]), m4_default([$4], [GW_]AS_TR_CPP([$1])), [$2], [$3])
])

AC_DEFUN([GW_CHECK_WAYLAND], [
    gw_wayland_wayland_min_version="1.1.91"

    _GW_CHECK([wayland], [wayland-client >= ${gw_wayland_wayland_min_version} $2], [errno.h $3], [$1])
])

AC_DEFUN([GW_CHECK_XCB], [
    _GW_CHECK([xcb], [xcb $2], [stdlib.h $3], [$1])
])

AC_DEFUN([GW_CHECK_MPD], [
    gw_mpd_gio_unix=
    PKG_CHECK_EXISTS([gio-unix-2.0], [gw_mpd_have_gio_unix=yes], [gw_mpd_have_gio_unix=no])
    AS_IF([test x${gw_mpd_have_gio_unix} = xyes], [
        gw_mpd_gio_unix="gio-unix-2.0"
    ])

    _GW_CHECK([mpd], [libmpdclient gobject-2.0 gio-2.0 ${gw_mpd_gio_unix} $2], [errno.h sys/types.h sys/socket.h $3], [$1])
])

AC_DEFUN([GW_CHECK_ALSA_MIXER], [
    _GW_CHECK([alsa-mixer], [alsa $2], [sys/poll.h $3], [$1])
])

AC_DEFUN([GW_CHECK_NL], [
    _GW_CHECK([nl], [libnl-3.0 $2], [linux/netlink.h $3], [$1])
])

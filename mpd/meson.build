mpd = dependency('libmpdclient', required: false, disabler: true)

libgwater_mpd_inc = include_directories('.')
libgwater_mpd_dep = [ mpd, gio, gio_platform, gobject, glib ]
libgwater_mpd_lib = static_library('libgwater-mpd', files(
        'libgwater-mpd.h',
        'libgwater-mpd.c',
    ),
    dependencies: libgwater_mpd_dep,
    include_directories: libgwater_mpd_inc,
    build_by_default: false,
)

libgwater_mpd = declare_dependency(link_with: libgwater_mpd_lib, include_directories: libgwater_mpd_inc, dependencies: libgwater_mpd_dep)

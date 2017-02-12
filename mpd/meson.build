project('libgwater-mpd', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'

foreach h : [ 'errno.h', 'sys/types.h', 'sys/socket.h' ]
    if not c_compiler.has_header(h)
        error('@0@ required but not found'.format(h))
    endif
glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
gobject = dependency('gobject-2.0')
gio = dependency('gio-2.0')
if host_machine.system() != 'windows'
    gio_platform = dependency('gio-unix-2.0')
else
    gio_platform = dependency('gio-windows-2.0')
end
mpd = dependency('libmpdclient')

libgwater_mpd_inc = include_directories('.')
libgwater_mpd_dep = [ mpd, gio, gio_platform, gobject, glib ]
libgwater_mpd_lib = static_library('libgwater-mpd', [
        'libgwater-mpd.h',
        'libgwater-mpd.c',
    ],
    dependencies: libgwater_mpd_dep,
    include_directories: libgwater_mpd_inc,
)

libgwater_mpd = declare_dependency(link_with: libgwater_mpd_lib, include_directories: libgwater_mpd_inc, dependencies: libgwater_mpd_dep)

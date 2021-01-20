project('libgwater', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'

glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
gobject = dependency('gobject-2.0')
gio = dependency('gio-2.0')
if host_machine.system() != 'windows'
    gio_platform = dependency('gio-unix-2.0')
else
    gio_platform = dependency('gio-windows-2.0')
endif

foreach h : [ 'errno.h', 'sys/types.h', 'sys/socket.h' ]
    if not c_compiler.has_header(h)
        error('@0@ required but not found'.format(h))
    endif
endforeach

subdir('wayland')
subdir('wayland-server')
subdir('xcb')
subdir('mpd')

if host_machine.system() == 'linux'
    subdir('nl')
    subdir('alsa-mixer')
endif

if host_machine.system() == 'windows'
    subdir('win')
endif

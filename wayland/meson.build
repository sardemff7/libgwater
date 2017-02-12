project('libgwater-wayland', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'
wayland_min_version='1.1.91'

if not c_compiler.has_header('errno.h')
    error('errno.h required but not found')
endif
glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
wayland = dependency('wayland-client', version: '>= @0@'.format(wayland_min_version))

libgwater_wayland_inc = include_directories('.')
libgwater_wayland_dep = [ wayland, glib ]
libgwater_wayland_lib = static_library('libgwater-wayland', [
        'libgwater-wayland.h',
        'libgwater-wayland.c',
    ],
    dependencies: libgwater_wayland_dep,
    include_directories: libgwater_wayland_inc,
)

libgwater_wayland = declare_dependency(link_with: libgwater_wayland_lib, include_directories: libgwater_wayland_inc, dependencies: libgwater_wayland_dep)

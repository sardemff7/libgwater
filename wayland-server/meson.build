project('libgwater-wayland-server', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'
wayland_min_version='1.1.91'

if not c_compiler.has_header('errno.h')
    error('errno.h required but not found')
endif
glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
wayland = dependency('wayland-server', version: '>= @0@'.format(wayland_min_version))

libgwater_wayland_server_inc = include_directories('.')
libgwater_wayland_server_dep = [ wayland, glib ]
libgwater_wayland_server_lib = static_library('libgwater-wayland-server', files(
        'libgwater-wayland-server.h',
        'libgwater-wayland-server.c',
    ),
    dependencies: libgwater_wayland_server_dep,
    include_directories: libgwater_wayland_server_inc,
)

libgwater_wayland_server = declare_dependency(link_with: libgwater_wayland_server_lib, include_directories: libgwater_wayland_server_inc, dependencies: libgwater_wayland_server_dep)

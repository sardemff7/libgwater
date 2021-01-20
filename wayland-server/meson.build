wayland_min_version='1.1.91'

wayland = dependency('wayland-server', version: '>= @0@'.format(wayland_min_version), required: false, disabler: true)

libgwater_wayland_server_inc = include_directories('.')
libgwater_wayland_server_dep = [ wayland, glib ]
libgwater_wayland_server_lib = static_library('libgwater-wayland-server', files(
        'libgwater-wayland-server.h',
        'libgwater-wayland-server.c',
    ),
    dependencies: libgwater_wayland_server_dep,
    include_directories: libgwater_wayland_server_inc,
    build_by_default: false,
)

libgwater_wayland_server = declare_dependency(link_with: libgwater_wayland_server_lib, include_directories: libgwater_wayland_server_inc, dependencies: libgwater_wayland_server_dep)

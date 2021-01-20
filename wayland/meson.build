wayland_min_version='1.1.91'

wayland = dependency('wayland-client', version: '>= @0@'.format(wayland_min_version), required: false, disabler: true)

libgwater_wayland_inc = include_directories('.')
libgwater_wayland_dep = [ wayland, glib ]
libgwater_wayland_lib = static_library('libgwater-wayland', files(
        'libgwater-wayland.h',
        'libgwater-wayland.c',
    ),
    dependencies: libgwater_wayland_dep,
    include_directories: libgwater_wayland_inc,
    build_by_default: false,
)

libgwater_wayland = declare_dependency(link_with: libgwater_wayland_lib, include_directories: libgwater_wayland_inc, dependencies: libgwater_wayland_dep)

project('libgwater-nl', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'

if not c_compiler.has_header('linux/netlink.h')
    error('linux/netlink.h required but not found')
endif
glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
libnl = dependency('libnl-3.0')

libgwater_nl_inc = include_directories('.')
libgwater_nl_dep = [ libnl, glib ]
libgwater_nl_lib = static_library('libgwater-nl', [
        'libgwater-nl.h',
        'libgwater-nl.c',
    ],
    dependencies: libgwater_nl_dep,
    include_directories: libgwater_nl_inc,
)

libgwater_nl = declare_dependency(link_with: libgwater_nl_lib, include_directories: libgwater_nl_inc, dependencies: libgwater_nl_dep)

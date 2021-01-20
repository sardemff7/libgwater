glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
libnl = dependency('libnl-3.0', required: false, disabler: true)
if libnl.found() and not c_compiler.has_header('linux/netlink.h')
    error('linux/netlink.h required but not found')
endif

libgwater_nl_inc = include_directories('.')
libgwater_nl_dep = [ libnl, glib ]
libgwater_nl_lib = static_library('libgwater-nl', files(
        'libgwater-nl.h',
        'libgwater-nl.c',
    ),
    dependencies: libgwater_nl_dep,
    include_directories: libgwater_nl_inc,
    build_by_default: false,
)

libgwater_nl = declare_dependency(link_with: libgwater_nl_lib, include_directories: libgwater_nl_inc, dependencies: libgwater_nl_dep)

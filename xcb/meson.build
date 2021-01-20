xcb = dependency('xcb', required: false, disabler: true)

libgwater_xcb_inc = include_directories('.')
libgwater_xcb_dep = [ xcb, glib ]
libgwater_xcb_lib = static_library('libgwater-xcb', files(
        'libgwater-xcb.h',
        'libgwater-xcb.c',
    ),
    dependencies: libgwater_xcb_dep,
    include_directories: libgwater_xcb_inc,
    build_by_default: false,
)

libgwater_xcb = declare_dependency(link_with: libgwater_xcb_lib, include_directories: libgwater_xcb_inc, dependencies: libgwater_xcb_dep)

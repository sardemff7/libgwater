project('libgwater-xcb', 'c')

glib_min_version='2.36'

glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
xcb = dependency('xcb')

libgwater_xcb_inc = include_directories('.')
libgwater_xcb_dep = [ xcb, glib ]
libgwater_xcb_lib = static_library('libgwater-xcb', files(
        'libgwater-xcb.h',
        'libgwater-xcb.c',
    ),
    dependencies: libgwater_xcb_dep,
    include_directories: libgwater_xcb_inc,
)

libgwater_xcb = declare_dependency(link_with: libgwater_xcb_lib, include_directories: libgwater_xcb_inc, dependencies: libgwater_xcb_dep)

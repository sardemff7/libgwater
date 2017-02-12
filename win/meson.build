project('libgwater-win', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'

if not c_compiler.has_header('windows.h')
    error('windows.h required but not found')
endif
glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))

libgwater_win_inc = include_directories('.')
libgwater_win_dep = [ glib ]
libgwater_win_lib = static_library('libgwater-win', [
        'libgwater-win.h',
        'libgwater-win.c',
    ],
    dependencies: libgwater_win_dep,
    include_directories: libgwater_win_inc,
)

libgwater_win = declare_dependency(link_with: libgwater_win_lib, include_directories: libgwater_win_inc, dependencies: libgwater_win_dep)

if not c_compiler.has_header('windows.h')
    error('windows.h required but not found')
endif

libgwater_win_inc = include_directories('.')
libgwater_win_dep = [ glib ]
libgwater_win_lib = static_library('libgwater-win', files(
        'libgwater-win.h',
        'libgwater-win.c',
    ),
    dependencies: libgwater_win_dep,
    include_directories: libgwater_win_inc,
    build_by_default: false,
)

libgwater_win = declare_dependency(link_with: libgwater_win_lib, include_directories: libgwater_win_inc, dependencies: libgwater_win_dep)

project('libgwater-alsa-mixer', 'c')

c_compiler = meson.get_compiler('c')

glib_min_version='2.36'

if not c_compiler.has_header('sys/poll.h')
    error('sys/poll.h required but not found')
endif
glib = dependency('glib-2.0', version: '>= @0@'.format(glib_min_version))
alsa = dependency('alsa')

libgwater_alsa_mixer_inc = include_directories('.')
libgwater_alsa_mixer_dep = [ alsa, glib ]
libgwater_alsa_mixer_lib = static_library('libgwater-alsa-mixer', files(
        'libgwater-alsa-mixer.h',
        'libgwater-alsa-mixer.c',
    ),
    dependencies: libgwater_alsa_mixer_dep,
    include_directories: libgwater_alsa_mixer_inc,
)

libgwater_alsa_mixer = declare_dependency(link_with: libgwater_alsa_mixer_lib, include_directories: libgwater_alsa_mixer_inc, dependencies: libgwater_alsa_mixer_dep)

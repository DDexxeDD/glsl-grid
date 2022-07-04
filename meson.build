project('voxelish test', 'c')

cc = meson.get_compiler('c')

sokol_dependencies = [
  cc.find_library('GL'),
  cc.find_library('m'),
  cc.find_library('dl'),
  cc.find_library('X11'),
  cc.find_library('Xi'),
  cc.find_library('Xcursor'),
  cc.find_library('pthread')
]

includes = [include_directories('.'),
  include_directories('include/sokol'),
  include_directories('include/stb'),
  include_directories('include/cglm'),
  include_directories('include/nuklear'),
  include_directories('include')
]

sources = [
  'source/main.c',
  'source/camera.c',
  'source/implementations.c',
]

grid_binary = executable('grid',
  sources,
  include_directories : includes,
  dependencies : sokol_dependencies)

project('trancas', 'cpp',
 version : '1.0',
 default_options : ['warning_level=3',  'buildtype=debugoptimized', 'cpp_std=c++17'])

boost_dep = dependency('boost')

project_sources = []

subdir('src')

executable('trancas',
 main_source,
 dependencies : boost_dep,
 install : true)

# This regex excludes any sources from the third_party, tests, benchmarks 
# and gtest related files.
regex = '^((?!(third_party|tests|benchmarks|gtest)).)*$'
# This adds clang tidy support
configure_file(input : '.clang-tidy',
               output : '.clang-tidy',
               copy : true)
run_target('tidy',
  command : ['run-clang-tidy.py','-fix', '-j' , '8', 'files', regex ,'-format', '-p='+ meson.build_root()])
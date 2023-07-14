project('abidump', ['c', 'cpp'], license: 'MIT', version: '1.0', default_options: [ 'buildtype=release', 'strip=true', 'warning_level=3' ])
src=[
  'abidump.c',
  'abidump_demangle.cc',
  'thirdparty/tiny-regex-c/re.c',
  'thirdparty/llvm/lib/Demangle/ItaniumDemangle.cpp'
]
executable('abidump', src, include_directories: ['thirdparty/llvm/include'], install: true)

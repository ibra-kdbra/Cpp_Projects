project('mage-hand', 'cpp', default_options: ['cpp_std=c++17'])

literals_generator = executable('generate-literal-operators', 'generate-literal-operators.cpp',
  override_options: ['cpp_std=c++17'],
  native: true)

literals_header = custom_target('literals-header',
  output: ['literals.h'],
  command: [literals_generator, '@OUTPUT@'],
  install: true,
  install_dir: get_option('includedir') + '/' + 'mage-hand')

main_test = executable('main-test', 'test.cpp', literals_header,
  override_options: ['cpp_std=c++17'],
  cpp_args: ['-Wno-literal-suffix', '-Wno-parentheses'])
test('Main test', main_test)

install_headers('comparisons.h', 'containers.h', 'type-traits.h', 'mage-hand.h', 'math.h',
  subdir: 'mage-hand')

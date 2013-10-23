SConscript('src/SConscript', variant_dir='build')
SConscript('test/cunit/SConscript', variant_dir='test/build')
env = Environment()
if 'docs' in COMMAND_LINE_TARGETS:
  env.Command('./docs/html/index.html', '', "doxygen")


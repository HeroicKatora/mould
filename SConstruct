# vim:ft=python
env = Environment(tools=['default', 'clang'])

env.MergeFlags(['-std=c++17', '-O3', '-DNDEBUG'])

if int(ARGUMENTS.get('clang', 0)):
    env.Replace(CXX='clang++')
if int(ARGUMENTS.get('profile', 0)): 
    env.Append(LINKFLAGS='-lprofiler')

SConscript('SConscript', variant_dir='build', exports='env')

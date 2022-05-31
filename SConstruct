# vim:ft=python
import os.path

env = Environment(tools=['default', 'clang'])
env.MergeFlags(['-std=c++17', '-O3', '-DNDEBUG', '-flto'])

def is_clang():
    return bool(int(ARGUMENTS.get('clang', 0)))

def is_profile():
    return bool(int(ARGUMENTS.get('profile', 0)))

if is_clang():
    env.Replace(CXX='clang++')
    env.Replace(CC='clang')
else:
    env.Replace(CXX='g++')
    env.Replace(CC='gcc')

env.Append(LINKFLAGS='-flto')

if is_profile(): 
    env.Append(LINKFLAGS='-lprofiler')

output_suffix = '{}-{}'.format(
        'clang' if is_clang() else 'gcc',
        'profile' if is_profile() else 'noprofile')

build_dir = os.path.join('build', output_suffix)
SConscript('SConscript', variant_dir=build_dir, exports='env')

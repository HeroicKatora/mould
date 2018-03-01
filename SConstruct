env = Environment(tools=['default', 'clang'], CXX='g++')

env.MergeFlags(['-std=c++17', '-O3'])
SConscript('SConscript', variant_dir='build', exports='env')

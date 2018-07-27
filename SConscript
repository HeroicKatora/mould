Import('env')

env.Append(CPPPATH='include')

dc_lib = SConscript('3rdparty/double-conversion.SConscript', exports='env')
env.Append(CPPPATH=[dc_lib.include])
dc_lib = dc_lib.static

ryu_lib = SConscript('3rdparty/ryu.SConscript', exports='env')
env.Append(CPPPATH=[ryu_lib.include])
ryu_lib = ryu_lib.static

env.Program('test/debug.cpp', LIBS=[dc_lib, ryu_lib])
env.Program('test/run.cpp', LIBS=[dc_lib, ryu_lib])
env.Program('test/speed.cpp', LIBS=[dc_lib, ryu_lib])

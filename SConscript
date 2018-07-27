Import('env')

dc_lib = SConscript('3rdparty/double-conversion.SConscript', exports='env').static
ryu_lib = SConscript('3rdparty/ryu.SConscript', exports='env')

env.Append(CPPPATH='include')
env.Program('test/debug.cpp', LIBS=[dc_lib, ryu_lib])
env.Program('test/run.cpp', LIBS=[dc_lib, ryu_lib])
env.Program('test/speed.cpp', LIBS=[dc_lib, ryu_lib])

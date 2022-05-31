Import('env')

env.Append(CPPPATH='include')

ryu_lib = SConscript('3rdparty/ryu.SConscript', exports='env')
env.Append(CPPPATH=[ryu_lib.include])
ryu_lib = ryu_lib.static

dragonbox_lib = SConscript('3rdparty/dragonbox.SConscript', exports='env')
env.Append(CPPPATH=[dragonbox_lib.include])
dragonbox_lib = dragonbox_lib.static

env.Program('test/debug.cpp', LIBS=[ryu_lib, dragonbox_lib])
env.Program('test/run.cpp', LIBS=[ryu_lib, dragonbox_lib])
env.Program('test/speed.cpp', LIBS=[ryu_lib, dragonbox_lib])

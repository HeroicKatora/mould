# vim:ft=python
Import('env')

ryu_sources = ['ryu/ryu/' + x for x in ('d2s.c', 'f2s.c')]
library_name = 'ryu'

ryu_static_objects = [
        env.StaticObject(src, CPPPATH=['ryu']) for src in ryu_sources]

ryu_library = env.StaticLibrary(library_name, ryu_static_objects)

Return('ryu_library')


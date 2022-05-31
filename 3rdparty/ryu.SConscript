# vim:ft=python
from collections import namedtuple
Import('env')

ryu_sources = ['ryu/ryu/' + x for x in (
		'd2s.c',
		'd2fixed.c',
		'f2s.c'
	)]
library_name = 'ryu'

ryu_static_objects = [
        env.StaticObject(src, CPPPATH=['ryu'], CXX='clang++') for src in ryu_sources]

Ryu = namedtuple('Ryu', 'static, include')
ryu_library = Ryu(
        static=env.StaticLibrary(library_name, ryu_static_objects, CXX='clang++'),
        include=Dir('ryu'))

Return('ryu_library')

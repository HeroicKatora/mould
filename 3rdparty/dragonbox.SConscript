# vim:ft=python
from collections import namedtuple
Import('env')

dragonbox_sources = ['dragonbox/src/' + x for x in (
		'dragonbox.cc',
	)]
library_name = 'dragonbox'

dragonbox_static_objects = [
        env.StaticObject(src, CPPPATH=['dragonbox/src']) for src in dragonbox_sources]

Dragonbox = namedtuple('Dragonbox', 'static, include')
dragonbox_library = Dragonbox(
        static=env.StaticLibrary(library_name, dragonbox_static_objects, CXX='clang++'),
        include=Dir('dragonbox/src'))

Return('dragonbox_library')

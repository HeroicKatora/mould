# vim:ft=python
from collections import namedtuple
Import('env')

# FIXME: this could be avoided by creating File-nodes in the called SConscript. Do this when we make a fork
double_conversion_sources = ['double-conversion/double-conversion/' + x for x in SConscript('double-conversion/double-conversion/SConscript')]
double_conversion_test_sources = ['double-conversion/test/cctest/' + x for x in SConscript('double-conversion/test/cctest/SConscript')]

double_conversion_shared_objects = [
    env.SharedObject(src, CPPPATH=['double-conversion']) for src in double_conversion_sources]
double_conversion_static_objects = [
    env.StaticObject(src, CPPPATH=['double-conversion']) for src in double_conversion_sources]

library_name = 'double-conversion'

static_lib = env.StaticLibrary(library_name, double_conversion_static_objects)
static_lib_pic = env.StaticLibrary(library_name + '_pic', double_conversion_shared_objects)
shared_lib = env.SharedLibrary(library_name, double_conversion_shared_objects)

DoubleConversion = namedtuple('DoubleConversion', 'static, pic, shared, include')
double_conversion_libs = DoubleConversion(
        static=static_lib,
        pic=static_lib_pic,
        shared=shared_lib,
        include=Dir('double-conversion'))
Return('double_conversion_libs')

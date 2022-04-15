from cffi import FFI
ffibuilder = FFI()

from sys import platform
print(platform)
if platform == "linux":
    libraries = ["glfw", "m", "dl"]
elif platform == "win32":
    libraries = ["glfw3"]

with open('../../include/glbt_interface.h', 'r') as file:
    ffibuilder.cdef(file.read())

ffibuilder.set_source("glbt",
"""
    #include "glbt.h"
""",
    include_dirs=['../../include', '../../include/internal'],
    sources=[
        '../../src/internal/glad.c',
        '../../src/buffer.c',
        '../../src/framebuffer.c',
        '../../src/pipeline.c',
        '../../src/texture.c',
        '../../src/window.c'
    ],
    libraries=libraries)

if __name__ == "__main__":
    ffibuilder.compile(verbose=True)

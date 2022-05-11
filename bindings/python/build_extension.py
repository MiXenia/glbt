import os, shutil, sys, glob
from cffi import FFI
ffibuilder = FFI()

os.chdir(os.path.dirname(__file__))

print(sys.platform)
if sys.platform == "linux":
    library_dirs = []
    libraries = ["glfw", "m", "dl"]
elif sys.platform == "win32":
    library_dirs = ["../../../lib/vs2022"]
    libraries = ["glfw3dll"]

with open('../../include/glbt_interface.h', 'r') as file:
    ffibuilder.cdef(file.read())

sources = glob.glob('../../src/**/*.c', recursive=True)
print(sources)

ffibuilder.set_source("glbt",
"""
    #include "glbt.h"
""",
    include_dirs=['../../../include', '../../../include/internal'],
    sources=['../'+src for src in sources],
    library_dirs=library_dirs,
    libraries=libraries)

if __name__ == "__main__":
    if os.path.exists('./out/'): shutil.rmtree('./out/') 
    for f in glob.glob('./*.so') + glob.glob('./*.pyd'):
        os.remove(f)
    output_file = ffibuilder.compile(tmpdir='./out/', verbose=True)
    print(output_file)
    shutil.copy(output_file, './')

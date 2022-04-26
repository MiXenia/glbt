from glbt import ffi
from glbt.lib import *

#these classes will make dealing with FFI easier so you don't have to.
class Pipeline:
	def __init__(self, vertex_shader_filename, fragment_shader_filename, inputs):
		vertex_shader = open(vertex_shader_filename)
		fragment_shader = open(fragment_shader_filename)
		vertex_shader_src = vertex_shader.read()
		fragment_shader_src = fragment_shader.read()
		vertex_shader.close()
		fragment_shader.close()

		vs_len = len(vertex_shader_src)
		fs_len = len(fragment_shader_src)
		self.vs = ffi.new("char[]", vertex_shader_src.encode('ascii'))
		self.fs = ffi.new("char[]", fragment_shader_src.encode('ascii'))

		self.vertex_attributes = []
		for i in inputs:
			self.vertex_attributes.append([i[0], ffi.new("char[]", i[1].encode('ascii')), i[2], i[3]])

		self.vertex_format = ffi.new("struct VertexInput[2]", self.vertex_attributes)

		self.pipeline = create_pipeline(vs_len, self.vs, fs_len, self.fs, len(self.vertex_attributes), self.vertex_format, 0, ffi.NULL)

	def __del__(self):
		destroy_pipeline(self.pipeline)

	def bind(self, o, location=-1):
		if isinstance(o, Buffer):
			bind_buffer(self.pipeline, o.buffer, location)
		elif isinstance(o, IndexBuffer):
			bind_index_buffer(self.pipeline, o.buffer)

	def run(self):
		run_pipeline(self.pipeline)

class Buffer:
	def __init__(self, data):
		self.data = ffi.new("float[]", data)
		self.buffer = create_buffer(len(data), self.data)
	def __del__(self):
		destroy_buffer(self.buffer)
class IndexBuffer:
	def __init__(self, indices):
		self.indices = ffi.new("unsigned int[]", indices)
		self.buffer = create_index_buffer(len(indices), self.indices)
	def __del__(self):
		destroy_buffer(self.buffer)



# here's the actual program.

win = create_window("UwU".encode('ascii'), 640, 400)

layout = [[0, "position", 1, 3], 
	    [1, "color", 1, 4]]
colored_polygons = Pipeline("shaders/color.glsl.vert", "shaders/color.glsl.frag", layout)

positions = Buffer([
	-0.5, 0.5, 0.0,
	0.5, 0.5, 0.0,
	0.5, -0.5, 0.0,
	-0.5, -0.5, 0.0
])
colors = Buffer([
	1.0, 0.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0
])
indices = IndexBuffer([0, 1, 2, 0, 2, 3])

colored_polygons.bind(positions, 0)
colored_polygons.bind(colors, 1)
colored_polygons.bind(indices)

while window_status(win) == RUNNING:
    if input_state(KEYBOARD_ESCAPE) == PRESSED:
        close_window(win)

    clear(1, 0, 1, 1)
    colored_polygons.run()

    refresh(win)
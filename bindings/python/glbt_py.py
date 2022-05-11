import os
from glbt import ffi
from glbt.lib import *

#these classes will make dealing with FFI easier so you don't have to.

class Window:
	def __init__(self, title, width, height):
		self.window = create_window(title.encode('ascii'), width, height)
	def status(self):
		return window_status(self.window)
	def close(self):
		close_window(self.window)
	def refresh(self):
		refresh(self.window)
	def screen(self):
		return screen(self.window)

class Pipeline:
	def __init__(self, vertex_shader_filename, fragment_shader_filename, inputs):
		vertex_shader = open(os.path.join(os.path.dirname(__file__), vertex_shader_filename))
		fragment_shader = open(os.path.join(os.path.dirname(__file__), fragment_shader_filename))
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

	def set_primitive_type(self, primitive, size):
		set_primitive_type(self.pipeline, primitive, size)
	def set_blend_state(self, state):
		set_blend_state(self.pipeline, state)

class Buffer:
	def __init__(self, data):
		self.data = ffi.new("float[]", data)
		self.buffer = create_buffer(len(data), self.data)
	def __del__(self):
		destroy_buffer(self.buffer)
	def update(self, data):
		self.data = ffi.new("float[]", data)
		buffer_data(self.buffer, len(data), self.data)
class IndexBuffer:
	def __init__(self, indices):
		self.indices = ffi.new("unsigned int[]", indices)
		self.buffer = create_index_buffer(len(indices), self.indices)
	def __del__(self):
		destroy_buffer(self.buffer)
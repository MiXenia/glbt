from glbt_py import *

# here's the actual program.

win = Window("UwU", 640, 400)

#list of inputs to the shader, each a matrix, [index, name, num_rows, num_columns]
layout = [
	[0, "position", 1, 3],
	[1, "color", 	1, 4]
]
colored_polygons = Pipeline("shaders/color.glsl.vert","shaders/color.glsl.frag", layout)
#colored_polygons.set_primitive_type(TRIANGLES, 16.0)
colored_polygons.set_blend_state(ADDITIVE_BLEND)

#create buffers which store data on the GPU itself so the shader can access it.
#in this case we're drawing a rectangle with 4 points
quad = [
	-0.5, 0.5, 0.0,
	0.5, 0.5, 0.0,
	0.5, -0.5, 0.0,
	-0.5, -0.5, 0.0
]
positions = Buffer(quad)
#each of the 4 points has an associated color, in RGBA
colors = Buffer([
	1.0, 0.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 0.0
])
#indices are to convert the points in the rectangle to 2 triangles.
indices = IndexBuffer([0, 1, 2, 0, 2, 3])

#bind these buffers to the shader pipeline at the specified points.
colored_polygons.bind(positions, 0)
colored_polygons.bind(colors, 1)
colored_polygons.bind(indices)

while win.status() == RUNNING:
	if input_state(KEYBOARD_ESCAPE) == PRESSED:
		win.close()

	if(input_state(MOUSE_BUTTON_LEFT) == DOWN):
		#heehee move the top-left corner of the quad with the mouse
		quad[0] = -1.0 + cursor_x()/320
		quad[1] = 1.0 - cursor_y()/200
		positions.update(quad)
	elif(input_state(MOUSE_BUTTON_LEFT) == RELEASED):
		quad[0] = -0.5
		quad[1] = 0.5
		positions.update(quad)

	clear(0, 0, 0, 1)
	colored_polygons.run()

	win.refresh()

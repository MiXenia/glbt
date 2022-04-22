#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in mat4 mvp;

out vec2 frag_texcoord;
out vec4 frag_color;

void main() {
	gl_Position = mvp * vec4(position, 1.0);
	frag_color = color;
	frag_texcoord = texcoord;
}

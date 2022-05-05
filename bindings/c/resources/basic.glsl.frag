#version 400
in vec4 frag_color;
in vec2 frag_texcoord;
out vec4 out_color;

uniform sampler2D tex;

void main() {
	out_color = /*frag_color * */texture(tex, frag_texcoord);
}

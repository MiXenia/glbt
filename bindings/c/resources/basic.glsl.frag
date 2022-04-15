#version 420
in vec4 frag_color;
in vec2 frag_texcoord;
out vec4 out_color;

layout(binding=0) uniform sampler2D tex;

void main() {
	out_color = frag_color * vec4(texture(tex, frag_texcoord).rgb, 1.0);
}

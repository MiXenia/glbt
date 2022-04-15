#version 420
in vec2 frag_texcoord;
out vec4 out_color;

layout(binding=0) uniform sampler2D color0;

uniform bool bnw;

void main() {
	vec3 color = texture(color0, frag_texcoord).rgb;
	if(bnw) {
		float average = (color.r + color.g + color.b)/3.0;
		out_color = vec4(average, average, average, 1.0);
	} else {
		out_color = vec4(color, 1.0);
	}
}
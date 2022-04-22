#version 400
out vec2 frag_texcoord;
void main() {
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    frag_texcoord = vec2((x+1.0)*0.5, (y+1.0)*0.5);
    gl_Position = vec4(x, y, 0, 1);
}

#version 330

layout(location = 0) in vec2 in_pos;

out VertexData {
    vec3 color;
} o;

void main() {
    o.color = vec3(in_pos.xy, 0.0);
    gl_Position = vec4(in_pos, 0.0, 1.0);
}

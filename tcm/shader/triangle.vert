#version 330

const vec2 view = vec2(9.0 / 16.0, 1.0);

layout(location = 0) in vec2 in_pos;

out VertexData {
    vec3 color;
} o;

void main() {
    o.color = vec3(in_pos.xy, 0.0);
    gl_Position = vec4(0.5 * view * in_pos, 0.0, 1.0);
}

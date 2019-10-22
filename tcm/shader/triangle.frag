#version 330

in VertexData {
    vec3 color;
} i;

out vec4 out_color;

void main() {
    out_color = vec4(i.color, 0.0);
}

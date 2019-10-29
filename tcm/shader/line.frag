#version 330

in VertexData {
    vec3 color;
} din;

out vec4 out_color;

void main() {
    out_color = vec4(din.color, 0.0);
}

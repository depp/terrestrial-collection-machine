#version 330

void main() {
    int idx = gl_VertexID - 1;
    float x = float(idx) / 8.0 - 1.0;
    gl_Position = vec4(x, sin(4.0 * x), 0.0, 1.0);
}

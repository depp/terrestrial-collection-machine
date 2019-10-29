#version 330

out VertexData {
    vec3 color;
} dout;

void main() {
    int idx = gl_VertexID - 1;
    float t = float(idx) / 16.0;
    float x = t * 2.0 - 1.0;
    dout.color = vec3(t, 0.5, 1.0 - t);
    gl_Position = vec4(x, sin(4.0 * x), 0.0, 1.0);
}

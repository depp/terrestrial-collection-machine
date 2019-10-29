#version 330

out VertexData {
    vec3 color;
} dout;

void main() {
    int idx = gl_VertexID - 1;
    vec2 v0 = vec2(-0.8, 0.0);
    vec2 v1 = vec2(0.8, 0.0);
    float t = float(idx) / 16.0;
    const int N = 4;
    int i;
    if (idx <= 0) {
    } else if (idx >= 1 << N) {
        v0 = v1;
    } else {
        float dir = 1.0;
        for (i = 0; i < N; i++) {
            vec2 v2 = mix(v0, v1, 0.5);
            v2 += (v1 - v0).yx * vec2(-1.0, 1.0) * 0.4 * dir;
            if ((idx & (1 << (3 - i))) == 0) {
                v1 = v2;
                dir = 1.0;
            } else {
                v0 = v2;
                dir = -1.0;
            }
        }
    }
    dout.color = vec3(t, 0.5, 1.0 - t);
    gl_Position = vec4(v0, 0.0, 1.0);
}

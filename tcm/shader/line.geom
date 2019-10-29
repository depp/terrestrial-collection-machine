#version 330

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

in VertexData {
    vec3 color;
} din[];

out VertexData {
    vec3 color;
} dout;

void main() {
    int i;
    vec2 lnorm[3];
    const float width = 0.1;
    for (i = 0; i < 3; i++) {
        vec2 d = (gl_in[i + 1].gl_Position - gl_in[i].gl_Position).yx;
        lnorm[i] = vec2(0.0);
        if (dot(d, d) > width * width) {
            lnorm[i] = normalize(d) * vec2(-1.0, 1.0);
        }
    }
    vec4 delta0 =
        vec4((lnorm[0] + lnorm[1]) / (1.0 + dot(lnorm[0], lnorm[1])), 0.0, 0.0);
    vec4 delta1 =
        vec4((lnorm[1] + lnorm[2]) / (1.0 + dot(lnorm[1], lnorm[2])), 0.0, 0.0);
    dout.color = din[1].color;
    gl_Position = gl_in[1].gl_Position - width * delta0;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + width * delta0;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position - width * delta1;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + width * delta1;
    EmitVertex();
    EndPrimitive();
}

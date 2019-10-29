#version 330

const vec2 view = vec2(9.0 / 16.0, 1.0);

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
    const float width = 0.02;
    for (i = 0; i < 3; i++) {
        vec2 d = (gl_in[i + 1].gl_Position - gl_in[i].gl_Position).yx;
        lnorm[i] = vec2(0.0);
        if (dot(d, d) > width * width) {
            lnorm[i] = normalize(d) * vec2(-1.0, 1.0);
        }
    }
    vec2 delta0 =
        width * (lnorm[0] + lnorm[1]) / (1.0 + dot(lnorm[0], lnorm[1]));
    vec2 delta1 =
        width * (lnorm[1] + lnorm[2]) / (1.0 + dot(lnorm[1], lnorm[2]));
    dout.color = din[1].color;
    gl_Position = vec4(view * (gl_in[1].gl_Position.xy - delta0), 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(view * (gl_in[1].gl_Position.xy + delta0), 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(view * (gl_in[2].gl_Position.xy - delta1), 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(view * (gl_in[2].gl_Position.xy + delta1), 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

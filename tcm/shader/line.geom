#version 330

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

void main() {
    const vec4 delta = vec4(0.1, 0.0, 0.0, 0.0);
    gl_Position = gl_in[1].gl_Position - delta;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + delta;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position - delta;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + delta;
    EmitVertex();
    EndPrimitive();
}

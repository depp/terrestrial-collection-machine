#version 330

layout(location = 0) in vec2 in_quad;
layout(location = 1) in vec2 in_pos;
layout(location = 2) in ivec4 in_glyph;

uniform vec2 scale;
uniform vec2 csize;
uniform vec2 tsize;

out VertexData {
    vec2 texcoord;
} dout;

void main() {
    dout.texcoord = (vec2(in_glyph.xy) + in_quad) * tsize;
    gl_Position =
        vec4(vec2(-1.0, 1.0) + scale * (in_quad * csize + in_pos), 0.0, 1.0);
}

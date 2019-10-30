#version 330

in VertexData {
    vec2 texcoord;
} din;

out vec4 out_color;

uniform sampler2D glyphs;

void main() {
    float a = texture(glyphs, din.texcoord).r;
    out_color = vec4(vec3(a), 1.0);
}

#version 330 core
out vec4 frag_color;

in vec3 out_color;
in vec2 out_tex_coords;

uniform sampler2D a_texture;
uniform vec2 offset;

void main() {
	frag_color = vec4(out_color, 1.0) * texture(a_texture, out_tex_coords + offset);
}

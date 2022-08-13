#version 330 core
out vec4 frag_color;

in vec3 out_color;
in vec2 out_tex_coords;

uniform sampler2D a_texture;
uniform vec2 offset;
uniform float alpha;

void main() {
	// Final Col = RGBA Output Col, Alpha * RGBA Color Of Texture At This Coordinate
	frag_color = vec4(out_color, alpha) * texture(a_texture, out_tex_coords + offset);
}

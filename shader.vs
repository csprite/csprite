#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coords;

out vec3 out_color;
out vec2 out_tex_coords;

void main() {
	gl_Position = vec4(position, 1.0);
	out_color = color;
	out_tex_coords = tex_coords;
}
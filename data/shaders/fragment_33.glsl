#version 330 // OpenGL v3.3

// This file is just compatible with opengl 3.3

out lowp vec4 frag_color;

in lowp vec3 out_color;
in mediump vec2 out_tex_coords;

uniform sampler2D a_texture;
uniform mediump vec2 offset;
uniform lowp float alpha;
uniform float patternSize;

void main() {
	float sampleAlpha = texture(a_texture, (out_tex_coords + offset).xy).w;
	if (sampleAlpha <= 0.0) {
		vec2 Pos = floor(gl_FragCoord.xy / patternSize);
		float PatternMask = mod(Pos.x + mod(Pos.y, 2.0), 2.0);
		frag_color = PatternMask * vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		// Final Col = RGBA Output Col, Alpha * RGBA Color Of Texture At This Coordinate
		frag_color = vec4(out_color, alpha) * texture(a_texture, out_tex_coords + offset);
	}
}

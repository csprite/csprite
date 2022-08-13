#version 130 // OpenGL v3.0

// Todo: Bug Fix - Black Color is not shown

/*
	lowp     -  Low Precision     (Best for colors)
	mediump  -  Medium Precision  (Best for Texture Co-ordinates)
	highp    -  High Precision    (Best for Vertex Positions)

	Many systems don't support highp, so we're using mediump
*/

out lowp vec4 frag_color;

in lowp vec3 out_color;
in mediump vec2 out_tex_coords;

uniform sampler2D a_texture;
uniform mediump vec2 offset;
uniform lowp float alpha;

void main() {
	vec3 chromaKeyColor = texture(a_texture, (out_tex_coords + offset).xy).xyz;
	if ((chromaKeyColor.x <= 0.01) && (chromaKeyColor.y <= 0.01) && (chromaKeyColor.z <= 0.01)){
		vec2 Pos = floor(gl_FragCoord.xy / 10.0); // 10.0 - Size of Squares
		float PatternMask = mod(Pos.x + mod(Pos.y, 2.0), 2.0);
		frag_color = PatternMask * vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		// Final Col = RGBA Output Col, Alpha * RGBA Color Of Texture At This Coordinate
		frag_color = vec4(out_color, alpha) * texture(a_texture, out_tex_coords + offset);
	}
}

#version 130

varying vec3 normal;
varying vec2 projCurvature;
varying vec2 projCenter;

uniform sampler2DArray brushes;
//uniform sampler2D brushes;

void main()
{	
		
	// Lightning calculations
	vec3 L = normalize(vec3(-1.0, 0.0, 1.0));
	vec3 N = normalize(normal);
	
	float intensity = max(dot(N, L), 0.0);
	//float intensity = 0.8;
	
	// Get texture coordinates and rotate according to the curvature projection
	vec2 inCoords = gl_TexCoord[0].st;
	
	float angle = -atan(projCurvature.s, projCurvature.t);
	mat2 rotation = mat2(
		cos(angle), sin(angle),  // first column (not row!)
		-sin(angle), cos(angle)
	);
	
	vec2 center = vec2(0.5, 0.5);
	inCoords = rotation * (inCoords - center) + center;
	
	// Previously : floor( (1.0-intensity) * 16.)
	vec4 color = texture(brushes, vec3(inCoords.xy, floor( (intensity) * 16.)).xyz); 

	gl_FragColor = color; // vec4(inCoords, floor((1.0-intensity) * 15.) / 15., 0.);
}
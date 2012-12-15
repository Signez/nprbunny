varying vec3 normal;
varying vec2 projCurvature;
varying vec2 projCenter;

uniform sampler2D texture; //the 1D texture that store the discrete values for the shading


void main()
{	
		
	// Lightning calculations
	vec3 L = normalize(vec3(-1.0, 0.0, 1.0));
	vec3 N = normalize(normal);
	
	float intensity = max(dot(N, L), 0.0);
	//float intensity = 0.8;
	
	// Get tex coordinates and rotate according to the curavture projection
	vec2 Coords = gl_TexCoord[0].st;
	
	float angle = -atan(projCurvature.s, projCurvature.t);
	mat2 rotation = mat2(
		cos(angle), sin(angle),  // first column (not row!)
		-sin(angle), cos(angle)
	);
	
	
	Coords = rotation * (Coords - gl_TexCoord[2].st) + gl_TexCoord[2].st;
	
	float index = (floor( (1.0-intensity) * 15.));
	Coords.x = Coords.x/4.0 + mod(index, 4.0) / 4.0 ;
	Coords.y = Coords.y/4.0  + floor(index/4.0) / 4.0;

	vec3 color = texture2D(texture, Coords).xyz;

	gl_FragColor = vec4(color, 1.0);
	
	 
	
}
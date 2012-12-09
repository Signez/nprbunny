varying vec3 normal;

uniform sampler2D texture; //the 1D texture that store the discrete values for the shading

void main()
{	
		
	vec3 L = vec3(0.0, 0.0, 1.0);
	vec3 N = normalize(normal);
	
	float intensity = max(dot(N, L), 0.0);

	vec2 Coords = gl_TexCoord[0].xy;
	float index = floor((1- intensity) * 15);
	Coords.x = Coords.x + floor(mod(index, 4.0)) / 4.0;
	Coords.y = Coords.y + floor(index / 4.0) / 4.0;
	
	
	
	vec3 color = texture2D(texture, Coords).xyz;

	gl_FragColor = vec4(color, 1.0);
	
}
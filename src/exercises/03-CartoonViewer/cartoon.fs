varying vec3 normal;

uniform sampler2D texture; //the 1D texture that store the discrete values for the shading

void main()
{	
		
	vec3 L = vec3(0.0, 0.0, 1.0);
	vec3 N = normalize(normal);
	
	float intensity = max(dot(N, L), 0.0);
	
	
	vec3 color = texture2D(texture, gl_TexCoord[0].xy).xyz;

	gl_FragColor = vec4(color, 1.0);
	
}
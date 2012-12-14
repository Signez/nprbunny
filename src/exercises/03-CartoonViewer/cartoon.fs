varying vec3 normal;
varying vec4 position;


uniform sampler2D texture; //the 1D texture that store the discrete values for the shading
uniform float width;
uniform float height;


void main()
{	
		
	vec3 L = vec3(0.0, 0.0, 1.0);
	vec3 N = normalize(normal);
	
	float intensity = max(dot(N, L), 0.0);
	//float intensity = 0.3;
	
	
	/*vec2 Coords = position.xy;
	Coords.x = (0.5 + 0.5 * Coords.x );
	Coords.y = (0.5 + 0.5 * Coords.y ); 
	*/
	
	vec2 Coords = gl_TexCoord[0].xy;
	
	float index = (floor( (1.-intensity) * 15.));
	Coords.x = Coords.x/4.0 + mod(index, 4.0) / 4.0 ;
	Coords.y = Coords.y/4.0  + floor(index/4.0) / 4.0;

	vec3 color = texture2D(texture, Coords).xyz;

	gl_FragColor = vec4(color, 1.0);
	
	 
	
}
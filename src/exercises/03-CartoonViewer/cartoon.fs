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
	//float intensity = .0;
	
	vec2 Coords = position.xy;
	Coords.x = (0.5 + 0.5 * Coords.x )*1000./width;
	Coords.y = (0.5 + 0.5 * Coords.y )*500./height; 
	float index = (floor( (intensity) * 16.));
	Coords.x = 1. - ( Coords.x + float(mod(index, 4.)) / 4.0 );
	Coords.y = ( Coords.y  + float(int(index) / 4) / 4.0);
	
	
	vec3 color = texture2D(texture, Coords).xyz;

	gl_FragColor = vec4(color, 1.0);
	
}
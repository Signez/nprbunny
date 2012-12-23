#version 130

varying float depth;
varying vec3 normal;
		
void main()
{	
	vec3 temp = vec3(0.5) + 0.5*normal;
	gl_FragColor = vec4(temp,depth);	
	
}
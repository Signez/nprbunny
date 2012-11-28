uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;
uniform mat3 worldcameraNormal;
uniform mat3 modelworldNormal;

uniform float near; // near plane
uniform float far; // far plane

varying float depth; // The varying that need to be set to the linear depth re-scaled between 0 and 1 
varying vec3 normal;

void main()
{	  
	// transform vertex to camera coordinates
	vec3 vertex = vec3( worldcamera * modelworld * gl_Vertex );
	 
	// compute the linear depth - store depth between 0 and 1
	depth = (-vertex.z-near)/(far-near);
	
	//compute the normals
	normal = normalize( worldcameraNormal * modelworldNormal * gl_Normal );
	
	// project the point into the camera
	gl_Position = projection * vec4( vertex, 1.0 );
	
}

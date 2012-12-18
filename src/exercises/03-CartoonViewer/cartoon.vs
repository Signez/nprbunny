#version 130

uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

varying vec3 normal;
varying vec2 projCurvature;
varying vec2 projCenter;

void main()
{	  
	// transform normal to camera coordinates
	normal = (worldcamera * modelworld * vec4( gl_Normal, 0.0) ).xyz;
	
	// transform vertex to camera coordinates
	vec3 vertex = vec3( worldcamera * modelworld * gl_Vertex );
	
	// get texture coordinate
	gl_TexCoord[0]  = gl_MultiTexCoord0;
	gl_TexCoord[1]  = gl_MultiTexCoord1; //curvature direction
	gl_TexCoord[2]  = gl_MultiTexCoord2;
	
	vec3 curv = gl_TexCoord[1].xyz;
	projCurvature = ( projection * vec4( curv, 0.0) ).xy;
	
	// project the point into the camera
	gl_Position = projection * vec4( vertex, 1.0 );
}

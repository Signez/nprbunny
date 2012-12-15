uniform sampler2D texture1; // the first pass texture
uniform sampler2D texture2; // the second pass texture
uniform sampler2D texture3; // the third pass texture
uniform sampler2D ShakeEdge; // the edge texture
uniform sampler2D Paper; // the paper texture
		
void main()
{	
	vec4 color1 = texture2D(texture1, gl_TexCoord[0].xy );
	vec4 color2 = texture2D(texture2, gl_TexCoord[0].xy );
	vec4 color3 = texture2D(texture3, gl_TexCoord[0].xy );
	
	// blend the 3 way rendering
	vec4 blended = ( color1 + color2 + color3 )/3.0;
	
	// The edges
	vec4 edgeColor = texture2D(ShakeEdge, gl_TexCoord[0].xy );
	
	// Paper color
	vec4 paperColor = texture2D(Paper, gl_TexCoord[0].xy );
	
	gl_FragColor = paperColor * blended * edgeColor;
	
}
uniform sampler2D texture; //the depth texture that need to be post processed with the sobel operator

uniform float dx; //use this uniform to move 1 pixel in x
uniform float dy; //use this uniform to move 1 pixel in y
	
float convulationnate(mat3 kernel) {
    float result = 0.;
	for(int m = 0; m < 3; m++) {
		for(int n = 0; n < 3; n++) {
			result += float(kernel[m][n] 
                   * texture2D(texture, vec2(gl_TexCoord[0].x + float(m - 1) * dx, gl_TexCoord[0].y + float(n - 1) * dy)));
		}
	}  
	return float(result);
}
		
void main()
{	
	mat3 xDerivation = mat3(
		-1., 0., 1.,
		-2., 0., 2.,
		-1., 0., 1.
	);
	
	mat3 yDerivation = mat3(
		1.,   2.,  1.,
		0.,   0.,  0.,
		-1., -2.,  -1.
	);
	
	float M = sqrt(pow(convulationnate(xDerivation), 2.0) + pow(convulationnate(yDerivation), 2.0));
	
    gl_FragColor.xyz = vec3(1. - 10. * M, 
					    1. - 10. * M, 
				    	1. - 10. * M);
				    	
	/* gl_FragColor = texture2D(texture, gl_TexCoord[0].xy); */
		
	//texture2D(texture, gl_TexCoord[0].xy);
	
}
#version 130

uniform sampler2D texture; //the depth texture that need to be post processed with the sobel operator

uniform float dx; //use this uniform to move 1 pixel in x
uniform float dy; //use this uniform to move 1 pixel in y
	
vec4 convulationnate(mat3 kernel) {
    vec4 result = vec4(0.);
	for(int m = 0; m < 3; m++) {
		for(int n = 0; n < 3; n++) {
			result += float(kernel[m][n])
                   * texture2D(texture, vec2(gl_TexCoord[0].x + float(m - 1) * dx, gl_TexCoord[0].y + float(n - 1) * dy));
		}
	}  
	return result;
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
	
	vec4 convx = convulationnate(xDerivation);
	vec4 convy = convulationnate(yDerivation);
	
	float M =
	  sqrt(pow(convx.x, 2.0) + pow(convy.x, 2.0))
	* sqrt(pow(convx.y, 2.0) + pow(convy.y, 2.0))
	* sqrt(pow(convx.z, 2.0) + pow(convy.z, 2.0))
	+ sqrt(pow(convx.w, 2.0) + pow(convy.w, 2.0));
	
	M = 1. - 10. * M;
	
    gl_FragColor = vec4(M, M, M, 1.);
}
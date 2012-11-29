uniform sampler2D texture; // the edge texture

		
void main()
{	
	int N = 5;

	vec2 displacement;
	vec4 edgeColor=vec4(0.);
	vec2 position = gl_TexCoord[0].xy ;
	float phi=0.;
	
	for( int i=0; i<N; i++ )
	{
		phi += 0.7;
		displacement.x = (0.002+phi/5000.)*(1.+cos(200.*position.x + phi));
		displacement.y = (0.002+phi/5000.)*(1.+cos(200.*position.y+ phi));

		edgeColor += texture2D(texture, gl_TexCoord[0].xy + displacement.xy);
	
	}
	
	edgeColor = edgeColor/float(N);
	
	gl_FragColor = edgeColor;
	
}
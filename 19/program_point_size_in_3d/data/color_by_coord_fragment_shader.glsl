varying vec4 projected_vertex;

void main()
{
	gl_FragColor=projected_vertex;
	gl_FragColor+=vec4(1.0,1.0,1.0,0.0);
	gl_FragColor/=vec4(2.0,2.0,2.0,1.0);
	gl_FragColor.w=1.0;
}

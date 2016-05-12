varying vec3 vertexOut;
void main()
{
	float pi=3.1415927;
	gl_FragColor=vec4(sin(vertexOut.x),sin(pi/2.0+vertexOut.y),sin(pi+vertexOut.z),1.0);
}

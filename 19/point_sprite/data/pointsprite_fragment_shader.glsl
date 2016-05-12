#version 120
varying vec4 colorOut;
void main()
{
	gl_FragColor=vec4(gl_PointCoord.xy,0,1);
}

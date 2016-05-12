#version 120
uniform sampler2D texture;

varying vec4 colorOut;
varying vec4 texCoordRangeOut;

void main()
{
	vec2 texCoord;
	texCoord.x=texCoordRangeOut[0]*(1.0-gl_PointCoord.x)
	          +texCoordRangeOut[2]*gl_PointCoord.x;
	texCoord.y=texCoordRangeOut[1]*(1.0-gl_PointCoord.y)
	          +texCoordRangeOut[3]*gl_PointCoord.y;

	gl_FragColor=colorOut*texture2D(texture,texCoord);
}

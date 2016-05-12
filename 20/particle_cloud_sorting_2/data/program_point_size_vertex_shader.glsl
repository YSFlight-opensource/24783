#version 120
attribute vec3 vertex;
attribute vec4 color;
attribute float pointSize;

uniform mat4 projection,modelView;
uniform float sizeInPixel;
uniform float sizeIn3d;
uniform float viewportHeight;

varying vec4 colorOut;

void main()
{
	colorOut=color;
	gl_Position=projection*modelView*vec4(vertex,1.0);

	vec4 shift;
	shift=modelView*vec4(vertex,1.0);
	shift.y=shift.y+pointSize/2;
	shift=projection*shift;

	float heightInScreen,heightInPixel;
	heightInScreen=abs(shift.y/shift.w-gl_Position.y/gl_Position.w);
	heightInPixel=sizeIn3d*viewportHeight*heightInScreen;

	gl_PointSize=pointSize*sizeInPixel+heightInPixel;
}

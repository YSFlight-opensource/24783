attribute vec3 vertex;
attribute vec2 offset;
attribute vec4 color;
attribute vec2 texCoord;

uniform mat4 projection,modelView;
uniform float offsetInView;
uniform float offsetInPixel;
uniform float viewportWidth;
uniform float viewportHeight;
uniform sampler2D texture2d;

varying vec4 colorOut;
varying vec2 texCoordOut;

void main()
{
	vec4 pos=modelView*vec4(vertex,1.0);
	pos.xy=pos.xy+offsetInView*offset;
	pos=projection*pos;

	vec2 offsetInScreen;
	offsetInScreen.x=offsetInPixel*offset.x/(viewportWidth/2.0)/pos.w;
	offsetInScreen.y=offsetInPixel*offset.y/(viewportHeight/2.0)/pos.w;

	pos.xy=pos.xy+offsetInScreen;

	colorOut=color;
	texCoordOut=texCoord;

	gl_Position=pos;
}

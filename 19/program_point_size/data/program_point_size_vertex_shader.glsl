attribute vec3 vertex;
attribute vec4 color;
attribute float pointSize;

uniform mat4 projection,modelView;

varying vec4 colorOut;

void main()
{
	colorOut=color;
	gl_Position=projection*modelView*vec4(vertex,1.0);
	gl_PointSize=pointSize;
}

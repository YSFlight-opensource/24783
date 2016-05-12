attribute vec2 texCoord;
attribute vec3 vertex;

varying vec2 texCoordOut;

void main()
{
	gl_Position=vec4(vertex,1.0);
	texCoordOut=texCoord;
}

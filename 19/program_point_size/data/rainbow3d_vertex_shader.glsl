attribute vec3 vertex;

uniform mat4 projection,modelView;

varying vec3 vertexOut;

void main()
{
	vertexOut=vertex;
	gl_Position=projection*modelView*vec4(vertex,1.0);
}

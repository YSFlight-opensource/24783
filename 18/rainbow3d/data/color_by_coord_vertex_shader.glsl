attribute vec3 vertex;

varying vec4 projected_vertex;

void main()
{
	projected_vertex=vec4(vertex,1.0);
	gl_Position=vec4(vertex,1.0);
}

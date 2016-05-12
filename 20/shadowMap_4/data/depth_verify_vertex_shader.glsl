#ifdef GL_ES
	#define LOWP lowp
	#define MIDP mediump
	#define HIGHP highp
#else
	#define LOWP
	#define MIDP
	#define HIGHP
#endif

attribute HIGHP vec2 vertex;
attribute HIGHP vec2 texCoord;

varying HIGHP vec2 texCoordOut;

void main()
{
	texCoordOut=texCoord;
	gl_Position=vec4(vertex,0.5,1.0);
}

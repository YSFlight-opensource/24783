uniform sampler2D texIdent;
varying vec2 texCoordOut;

void main()
{
	gl_FragColor=texture2D(texIdent,texCoordOut);
}

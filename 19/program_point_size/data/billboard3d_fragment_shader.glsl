uniform sampler2D texture2d;

varying vec4 colorOut;
varying vec2 texCoordOut;

void main()
{
	if(0.0<texCoordOut.x && texCoordOut.x<1.0 && 
	   0.0<texCoordOut.y && texCoordOut.y<1.0)
	{
		gl_FragColor=colorOut*texture2D(texture2d,texCoordOut);
	}
	else
	{
		discard;
	}
}

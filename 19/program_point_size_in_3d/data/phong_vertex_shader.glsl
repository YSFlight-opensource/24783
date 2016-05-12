attribute vec3 vertex;
attribute vec3 normal;
attribute vec4 color;

uniform mat4 projection,modelView;
uniform vec3 lightDir;
uniform float ambient;
uniform float specularIntensity;
uniform float specularExponent;

varying vec4 colorOut;
varying vec3 normalOut;
varying vec3 viewDirOut;

void main()
{
	vec4 posInView=modelView*vec4(vertex,1.0);
	viewDirOut=-normalize(posInView.xyz);
	normalOut=normalize((modelView*vec4(normal,0.0)).xyz);
	colorOut=color;
	gl_Position=projection*modelView*vec4(vertex,1.0);
}

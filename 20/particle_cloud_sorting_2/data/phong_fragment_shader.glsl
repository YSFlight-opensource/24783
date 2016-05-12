varying vec4 colorOut;
varying vec3 normalOut;
varying vec3 viewDirOut;

uniform vec3 lightDir;
uniform float ambient;
uniform float specularIntensity;
uniform float specularExponent;

void main()
{
	vec3 lit=normalize(lightDir);

	float diffuse=max(0.0,dot(normalOut,lit));

	vec3 midDir=normalize(viewDirOut+lightDir);
	float specular=specularIntensity*pow(max(0.0,dot(midDir,normalOut)),specularExponent);

	gl_FragColor=vec4(colorOut.rgb*(ambient+diffuse),colorOut.a)
            +vec4(specular,specular,specular,0.0);
}

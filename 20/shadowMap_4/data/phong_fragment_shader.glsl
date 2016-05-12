varying vec4 colorOut;
varying vec3 normalOut;
varying vec3 viewDirOut;

uniform vec3 lightDir;
uniform float ambient;
uniform float specularIntensity;
uniform float specularExponent;

uniform sampler2D shadowMapTexture;
varying vec4 shadowMapCoord;


vec4 RainbowColor(float t)
{
	// 0     0.25  0.5    0.75    1
	// Blue->Cyan->Green->Yellow->Red

	float tt;
	if(t<0.0)
	{
		return vec4(0,0,1,1);
	}
	else if(t<0.25)
	{
		tt=t/0.25;
		return vec4(0,tt,1,1);
	}
	else if(t<0.5)
	{
		tt=(t-0.25)/0.25;
		return vec4(0,1,1.0-tt,1);
	}
	else if(t<0.75)
	{
		tt=(t-0.5)/0.25;
		return vec4(tt,1,0,1);
	}
	else if(t<1.0)
	{
		tt=(t-0.75)/0.25;
		return vec4(1,1.0-tt,0,1);
	}
	else
	{
		return vec4(1,0,0,1);
	}
}

void main()
{
	vec3 lit=normalize(lightDir);

	float diffuse=max(0.0,dot(normalOut,lit));

	vec3 midDir=normalize(viewDirOut+lightDir);
	float specular=specularIntensity*pow(dot(midDir,normalOut),specularExponent);


	vec3 shadowCoordTfm=shadowMapCoord.xyz/shadowMapCoord.w;
	shadowCoordTfm=(shadowCoordTfm+vec3(1,1,1))/2.0;
	if(0.0<shadowCoordTfm.x && shadowCoordTfm.x<1.0 &&
	   0.0<shadowCoordTfm.y && shadowCoordTfm.y<1.0)
	{
		float depth;
		depth=texture2D(shadowMapTexture,shadowCoordTfm.xy).r;
		if(shadowCoordTfm.z>depth+0.001)
		{
			diffuse=0.0;
			specular=0.0;
		}
	}

	gl_FragColor=vec4(colorOut.rgb*(ambient+diffuse),colorOut.a)
            +vec4(specular,specular,specular,0.0);
}

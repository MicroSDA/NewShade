
vec4 BilinPhongDirectLight(vec3 normal, DirectLight light, Material material, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture, float shadow)
{
	vec4 ambientColor	 = vec4((diffuseTexture.rgb * material.AmbientColor * light.AmbientColor), diffuseTexture.a);
	vec4 diffuseColor	 = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specularColor	 = vec4(0.0, 0.0, 0.0, 0.0);

	float diffuseShading = dot(normal, -light.Direction);

	if(diffuseShading > 0.0)
	{
		diffuseColor			= vec4((diffuseTexture.rgb * material.DiffuseColor * light.DiffuseColor * diffuseShading), diffuseTexture.a);
		vec3 lightReflection	= reflect(light.Direction, normal);
		float specularShading	= dot(toCameraDirection, lightReflection);

		if(specularShading > 0.0)
		{
			specularShading		= pow(specularShading, material.Shinines);
			specularColor		= vec4((specularTexture.rgb * material.SpecularColor * light.SpecularColor * material.ShininesStrength * specularShading), specularTexture.a);
		}
	}

	return vec4(ambientColor + ((diffuseColor + specularColor) * shadow));
}

vec4 BilinPhongPointLight(vec3 normal, PointLight light, Material material, vec3 vertex, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture)
{
	vec3 lightDirection			= vertex - light.Position;
	float distanceBetween		= length(lightDirection);
	lightDirection				= normalize(lightDirection);
	// Probably bottle neck here
	DirectLight directLight;
	directLight.Direction 	 	= lightDirection;
	directLight.AmbientColor  	= light.AmbientColor;
	directLight.DiffuseColor  	= light.DiffuseColor;
	directLight.SpecularColor 	= light.SpecularColor;
  	//----------------------------------------------
	vec4 totalColor				= BilinPhongDirectLight(normal, directLight, material, toCameraDirection, diffuseTexture, specularTexture, 1.0);
	float attenuation       	= light.Constant + light.Linear * distanceBetween + light.Qaudratic * (distanceBetween * distanceBetween);

	return totalColor / attenuation;
}

vec4 BilinPhongSpotLight(vec3 normal, SpotLight light, Material material, vec3 vertex, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture)
{
	const float Smooth				= 2.0; // Harcoded 
    vec3  lightDirection			= normalize(vertex - light.Position);
    float spotFactor				= dot(lightDirection, light.Direction);
    
    if (spotFactor > light.MaxAngle)
    { 
        float epsilon				= light.MinAngle - light.MaxAngle;
        spotFactor					= smoothstep(0.0, Smooth, (spotFactor - light.MaxAngle) / epsilon);
        // Probably bottle neck here
        PointLight pointLight;
        pointLight.Position 		= light.Position;
        pointLight.AmbientColor 	= light.AmbientColor;
        pointLight.DiffuseColor 	= light.DiffuseColor;
        pointLight.SpecularColor 	= light.SpecularColor;
        pointLight.Constant 		= light.Constant;
        pointLight.Linear 			= light.Linear;
        pointLight.Qaudratic 		= light.Qaudratic;
        //--------------------------------------------
        vec4 totalColor				= BilinPhongPointLight(normal, pointLight, material, vertex, toCameraDirection, diffuseTexture, specularTexture);

        return totalColor * spotFactor;
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}
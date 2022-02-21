
vec4 BilinPhongDirectLight(vec3 normal, DirectLight light, Material material, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture, float shadow)
{
	vec4 ambientColor	 = vec4((diffuseTexture.rgb * material.AmbientColor), diffuseTexture.a);
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

	return vec4((ambientColor.rgb * light.Intensity) + vec3(diffuseColor.rgb + specularColor.rgb) * light.Intensity * shadow, diffuseTexture.a);
}

vec4 BilinPhongPointLight(vec3 normal, PointLight light, Material material, vec3 vertex, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture, float Shadow)
{
	vec3 lightDirection			= vertex - light.Position;
	float distanceBetween		= length(lightDirection);
	lightDirection				= normalize(lightDirection);
	// Probably bottle neck here
	DirectLight directLight;
	directLight.Direction 	 	= lightDirection;
	directLight.DiffuseColor  	= light.DiffuseColor;
	directLight.SpecularColor 	= light.SpecularColor;
	directLight.Intensity 		= light.Intensity;
  	//----------------------------------------------
	float attenuation  = clamp(1.0 - (distanceBetween * distanceBetween) / (light.Distance * light.Distance), 0.0, 1.0);
	attenuation       *= mix(attenuation, 1.0, light.Falloff );
	//attenuation        = attenuation * light.Falloff;
	
	vec4   totalColor = BilinPhongDirectLight(normal, directLight, material, toCameraDirection, diffuseTexture, specularTexture, Shadow);
	return totalColor * attenuation;
}

vec4 BilinPhongSpotLight(vec3 normal, SpotLight light, Material material, vec3 vertex, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture, float Shadow)
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
        pointLight.DiffuseColor 	= light.DiffuseColor;
        pointLight.SpecularColor 	= light.SpecularColor;
        pointLight.Intensity 		= light.Intensity;
        pointLight.Distance 		= light.Distance;
        pointLight.Falloff 			= light.Falloff;
        //--------------------------------------------
        vec4 totalColor				= BilinPhongPointLight(normal, pointLight, material, vertex, toCameraDirection, diffuseTexture, specularTexture, Shadow);
        return totalColor * spotFactor;
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}
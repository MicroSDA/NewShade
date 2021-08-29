// For BillinPhong lighting calculation
mat3 GetTBN_Matrix(mat4 ModelMatrix, vec3 Normal, vec3 Tangent)
{
	vec3 T 	= normalize(vec3(ModelMatrix * vec4(Tangent, 0.0)));
	vec3 N 	= normalize(vec3(ModelMatrix * vec4(Normal, 	0.0)));
		 T 	= normalize(T - dot(T, N) * N);
	vec3 B 	= normalize(cross(T, N));
	// Don't remember for what 
	if (dot(cross(N, T), B) > 0.0)
             T = T * -1.0;

	return mat3(T, B, N);
}
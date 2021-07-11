
mat3 CalculateTBNMatrix(mat4 modelMatrix, vec3 normal, vec3 tangent)
{
	vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(T, N));

	if (dot(cross(N, T), B) > 0.0)
             T = T * -1.0;

	return mat3(T, B, N);
}
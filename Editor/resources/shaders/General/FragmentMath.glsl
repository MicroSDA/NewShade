// For BillinPhong lighting calculation
vec3 Get_TBNNormal(vec3 Normal, mat3 TBN_Matrix)
{
   vec3 TBN_Normal = Normal;
   TBN_Normal = 2.0 * TBN_Normal  - vec3(1.0, 1.0, 1.0);   
   TBN_Normal = normalize(TBN_Matrix * TBN_Normal);
   return TBN_Normal;
}
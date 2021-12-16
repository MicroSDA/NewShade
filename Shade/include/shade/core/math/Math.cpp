#include "shade_pch.h"
#include "Math.h"

void shade::math::DecomposeMatrix(const glm::mat4& matrix, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
	/* Is overkill ?*/
	glm::quat r;
	glm::vec3 s;
	glm::vec4 p;
	glm::decompose(matrix, scale, r, translation, s, p);
	rotation = glm::eulerAngles(r);
}

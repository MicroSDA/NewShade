#include "shade_pch.h"
#include "Shadows.h"

shade::Shadows::Cascade shade::Shadows::ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction ,const float& nearPlane, const float& farplane, const float& split)
{
	glm::mat4 projection = glm::perspective(camera->GetFov(), camera->GetAspect(), nearPlane, farplane);

	auto frustumCorners = GetCameraFrustumCorners(projection, camera->GetView());
	glm::vec3 frustumCenter(0.0f);
	/* Get frustum center */
	for (const auto& c : frustumCorners)
	{
		frustumCenter += glm::vec3(c);
	}
	frustumCenter /= 8.0f; // 8

	float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::min(),
		  minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::min(),
		  minZ = std::numeric_limits<float>::max(), maxZ = std::numeric_limits<float>::min();

	/* Create veiw matrix based on camera frustum and light direction */			            /* Up vector */
	const glm::mat4 lightView = glm::lookAt(frustumCenter - direction, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

	for (const auto& c : frustumCorners)
	{
		const auto trf = lightView * c;
		minX = std::min(minX, trf.x); maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y); maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z); maxZ = std::max(maxZ, trf.z);
	}

	constexpr float zMult = 1.0f;
	if (minZ < 0)
	{
		minZ *= zMult;
	}
	else
	{
		minZ /= zMult;
	}

	if (maxZ < 0)
	{
		maxZ /= zMult;
	}
	else
	{
		maxZ *= zMult;
	}
	/* Create ortho projection based on camera frustum corners */
	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
	return  Cascade{ lightProjection * lightView, split };
}

std::vector<glm::vec4> shade::Shadows::GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw)
{
	/* Create new view projection based on the split distance*/
	glm::mat4 cameraViewProjection = glm::inverse(projection * veiw);

	/* Get camera frustum corners */
	std::vector<glm::vec4> corners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt = cameraViewProjection * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				corners.push_back(pt / pt.w);
			}
		}
	}
	return corners;
}

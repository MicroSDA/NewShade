#pragma once
#include "shade/config/API.h"
#include "shade/core/camera/Camera.h"
#include "shade/core/components/Components.h"
#include <glm/glm.hpp>

namespace shade
{
	class SHADE_API Shadows
	{
	public:
		struct Cascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16) float     ClipDistacne = 0.0f;
		};
		
	public:
		static Cascade ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction,   const float& nearPlane, const float& farplane, const float& split);
		static glm::mat4 ComputeSpotLightViewMatrix(const shade::Shared<Camera>& camera, const glm::vec3& position, const glm::vec3& direction);
	private:
		/* In world space */
		static std::vector<glm::vec4>  GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw);
	};
}
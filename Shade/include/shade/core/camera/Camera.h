#pragma once
#include "shade/config/API.h"
#include "shade/core/camera/Frustum.h"

namespace shade
{
	class SHADE_API Camera
	{
	public:
		struct RenderData
		{
			glm::mat4 ViewProjection;
			glm::mat4 View;
			glm::mat4 Projection;
			glm::vec3 Position;
			glm::vec3 Forward;
		};
	public:
		Camera();
		Camera(const glm::vec3& position,
			const float& fov,
			const float& aspect,
			const float& zNear,
			const float& zFar);
		virtual ~Camera() = default;
		inline RenderData GetRenderData() const { return RenderData{ GetViewProjection(), GetView(), GetProjection(), GetPosition(), GetForwardDirrection()}; }
		inline glm::mat4 GetView()                     const { return glm::lookAt(m_Position, m_Position + m_Forward, m_Up); };
		inline glm::mat4 GetView() { return glm::lookAt(m_Position, m_Position + m_Forward, m_Up); };
		inline const glm::mat4& GetProjection()        const { return m_Perpective; };
		inline glm::mat4 GetViewProjection()           const { return m_Perpective * glm::lookAt(m_Position, m_Position + m_Forward, m_Up); }
		inline const glm::vec3& GetForwardDirrection() const { return m_Forward; }
		inline glm::vec3& GetForwardDirrection() { return const_cast<glm::vec3&>(const_cast<const shade::Camera*>(this)->GetForwardDirrection()); }
		inline const glm::vec3& GetUpDirrection()      const { return m_Up; }
		inline const glm::vec3& GetPosition()          const { return m_Position; }
		inline glm::vec3& GetPosition() { return const_cast<glm::vec3&>(const_cast<const shade::Camera*>(this)->GetPosition()); }
		inline const float& GetFov()                   const { return m_Fov; }
		inline float& GetFov() { return const_cast<float&>(const_cast<const shade::Camera*>(this)->GetFov()); }
		inline const float& GetAspect()                const { return m_Aspect; }
		inline float& GetAspect() { return const_cast<float&>(const_cast<const shade::Camera*>(this)->GetAspect()); }
		inline const float& GetNear()				   const { return m_zNear; }
		inline float& GetNear() { return const_cast<float&>(const_cast<const shade::Camera*>(this)->GetNear()); }
		inline const float& GetFar()				   const { return m_zFar; }
		inline float& GetFar() { return const_cast<float&>(const_cast<const shade::Camera*>(this)->GetFar()); }

		inline void SetPosition(const float& x, const float& y, const float& z) { m_Position = glm::vec3(x, y, z); }
		inline void SetPosition(const glm::vec3& position) { m_Position = position; }
		inline void SetDirection(const glm::vec3& direction) { m_Forward = direction; }
		inline void MoveForward(const float& value) { m_Position += m_Forward * value; }
		inline void MoveBack(const float& value) { m_Position -= m_Forward * value; }
		inline void MoveRight(const float& value) { m_Position -= glm::cross(m_Up, m_Forward) * value; }
		inline void MoveLeft(const float& value) { m_Position += glm::cross(m_Up, m_Forward) * value; }
		inline void SetAspect(const float& aspect) { m_Aspect = aspect; _RecalculatePerpective(); }
		inline void SetFov(const float& fov) { m_Fov = fov; _RecalculatePerpective();}
		inline void SetNear(const float& zNear) { m_zNear = zNear; _RecalculatePerpective();}
		inline void SetFar(const float& zFar) { m_zFar = zFar; _RecalculatePerpective();}

		inline Frustum GetFrustum() const { return Frustum(GetView(), GetProjection()); };
		// Counter clockwise issue here
		// Y
		inline void RotateYaw(float angle)
		{
			glm::mat4 rotation = glm::rotate(glm::radians(angle), UP);
			m_Forward = glm::vec3(glm::normalize(rotation * glm::vec4(m_Forward, 0.0f)));
			m_Up = glm::vec3(glm::normalize(rotation * glm::vec4(m_Up, 0.0f)));
		}
		// X
		inline void RotatePitch(float angle)
		{
			glm::vec3 right = glm::normalize(glm::cross(m_Up, m_Forward));
			m_Forward = glm::vec3(glm::normalize(glm::rotate(glm::radians(-angle), right) * glm::vec4(m_Forward, 0.0)));
			m_Up = glm::normalize(glm::cross(m_Forward, right));
		}
		// Z
		inline void SetRoll(float angle) { /* TODO:*/ }
		void Resize(const float& aspect = 0);

		void SetAsPrimary(const bool& isPrimary);
		const bool& IsPrimary() const { return m_IsPrimary; }
	private:
		void _RecalculatePerpective();
		const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f); // Y is up
		glm::mat4 m_Perpective;
		glm::vec3 m_Position;
		glm::vec3 m_Forward;
		glm::vec3 m_Up;
		float	m_Fov, m_Aspect, m_zNear, m_zFar;
		bool	m_IsPrimary = false;
	};
}
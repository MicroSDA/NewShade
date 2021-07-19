#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/serializing/Serializable.h"
#include "shade/core/utils/Log.h"

namespace shade
{
	class SHADE_API Shader : public Serializable
	{
	public:
		enum class Type
		{
			Undefined,
			Vertex,
			Fragment,
			Geometry
		};
	public:
		virtual ~Shader() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SendInt(const std::string& name,		const int& value) = 0;
		virtual void SendInt2(const std::string& name,		const int* value) = 0;
		virtual void SendInt3(const std::string& name,		const int* value) = 0;
		virtual void SendInt4(const std::string& name,		const int* value) = 0;
		virtual void SendFlaot(const std::string& name,		const float& value) = 0;
		virtual void SendFlaot2(const std::string& name,	const float* value) = 0;
		virtual void SendFlaot3(const std::string& name,	const float* value) = 0;
		virtual void SendFlaot4(const std::string& name,	const float* value) = 0;
		virtual void SendMat3(const std::string& name,		const bool& isTransponse, const float* value) = 0;
		virtual void SendMat4(const std::string& name,		const bool& isTransponse, const float* value) = 0;
		virtual int GetUniformLocation(const std::string& name) const = 0;
		static Shared<Shader> Create(const std::string& filePath);
		virtual void Recompile() = 0;
	protected:
		std::unordered_map<Type, std::string> m_SourceCode;
		bool Parse(const std::string& source);
		Type GetTypeFromString(const std::string& type);
		void FindInclude(std::string& source);

		std::string m_Path;
	};
}

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
			Geometry,
			Compute
		};
	public:
		virtual ~Shader() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual std::uint32_t GetProgram() = 0;
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
		virtual void ExecuteSubrutines() = 0;
		virtual void DispatchCompute(const std::uint32_t& x, const std::uint32_t& y, const std::uint32_t& z) = 0;
		virtual void SelectSubrutine(const std::string& sUniformName, const std::string& subrutine, const Shader::Type& type) = 0;
		virtual int GetUniformLocation(const std::string& name) const = 0;
		static Shared<Shader> Create(const std::string& filePath);
		virtual void Recompile() = 0;

	protected:
		std::unordered_map<Type, std::string> m_SourceCode;
		bool Parse(const std::string& source);
		Type GetTypeFromString(const std::string& type);
		void FindInclude(std::string& source);
		std::string m_Path;
		// Subrutines
		std::vector<std::uint32_t> m_VertexSubrIndices, m_FragemntSubrIndices, m_GeometrySubrIndices, m_ComputeSubrIndices;
	protected:
	};

	class SHADE_API ShadersLibrary
	{
	public:
		static void Create(const std::string& name, const std::string& filepath);
		static Shared<Shader> Get(const std::string& name);
		static void Remove(const std::string& name);
	private:
		static std::unordered_map<std::string, Shared<Shader>> m_sLibrary;
	};
}

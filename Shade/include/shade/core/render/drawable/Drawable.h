#pragma once
#include "shade/config/API.h"
#include "shade/core/render/buffers/VertexArray.h"
#include "shade/core/mesh/Material3D.h"
#include "shade/core/image/Texture.h"
namespace shade
{
	class SHADE_API Drawable
	{
	public:
		enum class DrawMode
		{
			Points			= 0,
			Lines			= 1,
			LineStrip		= 2,
			LineLoop		= 3,
			Triangles		= 4,
			TriangleStrip	= 5,
			TriangleFan		= 6,
		};
	public:
		virtual ~Drawable() = default;

		void SetDrawMode(const DrawMode& mode);
	
		void AddVertex(const Vertex3D& vertex);
		void AddIndex(const Index& index);
		void AddVertices(Vertices& vertices);
		void AddIndices(Indices& indices);
		void SetVertices(Vertices& vertices);
		void SetIndices(Indices& indices);
		void SetMaterial(const Shared<Material3D>& material);
		
		const Vertices& GetVertices() const;
		const Indices& GetIndices()  const;
		const Shared<Material3D>& GetMaterial() const;

		Vertices&	GetVertices();
		Indices&	GetIndices();
		Shared<Material3D>&	GetMaterial();

		/* TODO swith to physics or so in future */
		void GenerateHalfExt();
		const glm::vec3& GetMinHalfExt() const;
		const glm::vec3& GetMaxHalfExt() const;

		void SetMinHalfExt(const glm::vec3& ext);
		void SetMaxHalfExt(const glm::vec3& ext);

		const DrawMode& GetDrawMode() const;
	private:
		DrawMode			 m_DrawMode = DrawMode::Triangles;
		Vertices			 m_Vertices;
		Indices				 m_Indices;
		Shared<Material3D>	 m_Material;

		glm::vec3			 m_MinHalfExt = glm::vec3(-1.0f);
		glm::vec3			 m_MaxHalfExt = glm::vec3(1.0f);
	};
}
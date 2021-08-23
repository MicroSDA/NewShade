#pragma once
#include "shade/config/API.h"
#include "shade/core/render/buffers/VertexArray.h"
#include "shade/core/mesh/Material.h"
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
		void SetMaterial(const Material& material);
		void AddTexture(const Shared<Texture>& texture); // TODO Remove or so

		const Vertices& GetVertices() const;
		const Indices& GetIndices()  const;
		const Material& GetMaterial() const;

		Vertices&	GetVertices();
		Indices&	GetIndices();
		Material&	GetMaterial();

		void GenerateHalfExt();
		const glm::vec3& GetMinHalfExt();
		const glm::vec3& GetMaxHalfExt();

		const DrawMode& GetDrawMode() const;
	private:
		DrawMode			 m_DrawMode = DrawMode::Triangles;
		Vertices			 m_Vertices;
		glm::vec3			 m_MinHalfExt;
		glm::vec3			 m_MaxHalfExt;
		Indices				 m_Indices;
		Material			 m_Material;
	};

}
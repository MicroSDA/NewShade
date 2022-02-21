#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Assimp::Importer importer;
//const aiScene* m_pScene = importer.ReadFile("",
//	aiProcess_FlipUVs |
//	aiProcess_Triangulate |
//	aiProcess_JoinIdenticalVertices |
//	aiProcess_CalcTangentSpace |
//	aiProcess_FixInfacingNormals |
//	aiProcess_GenSmoothNormals);
#include <shade/core/mesh/Model3D.h>

class IModel3D : public shade::Model3D
{
public:
	static shade::Shared<shade::Model3D> Import(const std::string& filePath);
	virtual ~IModel3D() = default;
private:
	void ProcessModel3DNode(const char* filePath, const aiNode* node, const aiScene* scene);
	void ProcessModel3DMesh(const char* filePath, aiMesh* mesh, const aiScene* scene);
	void ProcessTexture(const shade::Shared<shade::Material3D>& material, const std::string& filePath, aiMaterial* aImaterial, const aiTextureType& type);
	void ProcessMaterial(const shade::Shared<shade::Material3D>& material, const std::string& filePath, aiMaterial* aImaterial);
};


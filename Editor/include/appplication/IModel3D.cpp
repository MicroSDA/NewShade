#include "IModel3D.h"

shade::Shared<shade::Model3D> IModel3D::Import(const std::string& filePath)
{

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_FlipUVs |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_FixInfacingNormals |
		aiProcess_GenSmoothNormals);

	if (!pScene)
	{
		SHADE_WARNING("Failed to import model: {0}", filePath);
		return shade::Shared<shade::Model3D>();
	}
	else
	{
		auto model = shade::CreateShared<IModel3D>();
		model->ProcessModel3DNode(filePath.c_str(), pScene->mRootNode, pScene);
		return model;

	}
}

void IModel3D::ProcessModel3DNode(const char* filePath, const aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		ProcessModel3DMesh(filePath, mesh, scene);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessModel3DNode(filePath, node->mChildren[i], scene); // TODO THERE ! how to return if static 
	}
}

void IModel3D::ProcessModel3DMesh(const char* filePath, aiMesh* mesh, const aiScene* scene)
{
	auto _mesh = shade::CreateShared<shade::Mesh>();
	_mesh->GetVertices().reserve(mesh->mNumVertices);
	_mesh->GetIndices().reserve(mesh->mNumFaces); // Not sure that resevitaion in inidices case is working

	for (auto i = 0; i < mesh->mNumVertices; i++)
	{
		shade::Vertex3D vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent.x = mesh->mTangents[i].x;
				vertex.Tangent.y = mesh->mTangents[i].y;
				vertex.Tangent.z = mesh->mTangents[i].z;
			}
		}

		if (mesh->HasTextureCoords(0))
		{
			vertex.UV_Coordinates.x = mesh->mTextureCoords[0][i].x;
			vertex.UV_Coordinates.y = mesh->mTextureCoords[0][i].y;
		}

		_mesh->GetVertices().push_back(vertex);
	}

	for (auto i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; j++)
		{
			_mesh->GetIndices().push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial*  assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiString     texturePath;

		
		ProcessTexture(_mesh, filePath, assimpMaterial, aiTextureType_DIFFUSE);
		ProcessTexture(_mesh, filePath, assimpMaterial, aiTextureType_SPECULAR);
		ProcessTexture(_mesh, filePath, assimpMaterial, aiTextureType_HEIGHT);
		ProcessMaterial(_mesh, assimpMaterial);
	}

	m_Meshes.push_back(_mesh);
}

void IModel3D::ProcessTexture(const shade::Shared<shade::Mesh>& mesh, const std::string& filePath, aiMaterial* material, const aiTextureType& type)
{
	for (auto i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString path;
		std::string _path;
		material->GetTexture(type, i, &path);
		_path = std::filesystem::path(filePath).parent_path().string() + "\\" + path.C_Str();
		
		/*if (std::filesystem::exists(_path))
		{*/
			auto texture = shade::Texture::Create<shade::Texture>();
			texture->GetAssetData().Attribute("id").set_value(std::filesystem::path(_path).stem().string().c_str());
			texture->GetAssetData().Attribute("path").set_value(_path.c_str());
			texture->GetAssetData().Attribute("category").set_value("secondary");
			texture->GetAssetData().Attribute("type").set_value("texture");
			texture->GetAssetData()._Raw().append_attribute("texture_type");
			switch (type)
			{
			case aiTextureType::aiTextureType_DIFFUSE:
				texture->GetAssetData().Attribute("texture_type").set_value("diffuse");
				break;
			case aiTextureType::aiTextureType_SPECULAR:
				texture->GetAssetData().Attribute("texture_type").set_value("specular");
				break;
			case aiTextureType::aiTextureType_HEIGHT:
				texture->GetAssetData().Attribute("texture_type").set_value("normal_map");
				break;
			}
			
			if (!texture->Deserialize())
				SHADE_WARNING("");

			texture->AssetInit();
			//mesh->AddTexture(texture);
		//}
		
	}
}

void IModel3D::ProcessMaterial(const shade::Shared<shade::Mesh>& mesh, aiMaterial* material)
{
	//aiColor3D color;
	//mesh->SetMaterial(shade::Material());
	//material->Get(AI_MATKEY_COLOR_AMBIENT, color); // Ka
	//mesh->GetMaterial().SetAmbientColor(color.r, color.g, color.b);
	//material->Get(AI_MATKEY_COLOR_DIFFUSE, color);// Kd
	//mesh->GetMaterial().SetDiffuseColor(color.r, color.g, color.b);
	//material->Get(AI_MATKEY_COLOR_SPECULAR, color);// Ks
	//mesh->GetMaterial().SetSpecularColor(color.r, color.g, color.b);
	//material->Get(AI_MATKEY_COLOR_TRANSPARENT, color);// Tf
	//mesh->GetMaterial().SetTransparentMask(color.r, color.g, color.b);

	//float value;
	//material->Get(AI_MATKEY_SHININESS, value); // Ns
	//mesh->GetMaterial().SetShininess(value);
}

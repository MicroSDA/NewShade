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
		model->GetAssetData().Attribute("Id").set_value(std::filesystem::path(filePath).stem().string().c_str());
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
	/* Create asset data */
	_mesh->GetAssetData().Attribute("Id").set_value(mesh->mName.C_Str());
	_mesh->GetAssetData().Attribute("Type").set_value("Mesh");
	GetAssetData().Append(_mesh->GetAssetData());

	_mesh->GetVertices().reserve(mesh->mNumVertices);
	_mesh->GetIndices().reserve(mesh->mNumFaces); // Not sure that resevitaion in inidices case is working

	if (mesh->HasBones())
	{
		SHADE_CORE_INFO("Mesh has bones : {0} -----------------------------", mesh->mName.C_Str());
		for (auto i = 0; i < mesh->mNumBones; i++)
		{
			SHADE_CORE_INFO("---Bone name : {0}", mesh->mBones[i]->mName.C_Str());
		}
		SHADE_CORE_INFO("-----------------------------");
	}

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
		aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiString     texturePath;

		auto _material = shade::Material3D::Create<shade::Material3D>();
		_mesh->SetMaterial(_material);
		ProcessMaterial(_material, filePath, assimpMaterial);
		_mesh->GetAssetData().Append(_material->GetAssetData());

		ProcessTexture(_material,  filePath, assimpMaterial, aiTextureType_DIFFUSE);
		ProcessTexture(_material,  filePath, assimpMaterial, aiTextureType_SPECULAR);
		ProcessTexture(_material,  filePath, assimpMaterial, aiTextureType_HEIGHT);
		
	}

	_mesh->GenerateHalfExt();
	m_Meshes.push_back(_mesh);
}

void IModel3D::ProcessTexture(const shade::Shared<shade::Material3D>& material, const std::string& filePath, aiMaterial* aImaterial, const aiTextureType& type)
{
	for (auto i = 0; i < aImaterial->GetTextureCount(type); i++)
	{
		aiString path;  aImaterial->GetTexture(type, i, &path);
		std::filesystem::path _path = std::filesystem::path(filePath).parent_path().string() + "\\" + path.C_Str();
		auto texture = shade::Texture::Create<shade::Texture>();
		auto& assetData = texture->GetAssetData();

		assetData.Attribute("Id").set_value(_path.stem().string().c_str());
		assetData.Attribute("Path").set_value(_path.string().c_str());
		assetData.Attribute("Category").set_value("Secondary");
		assetData.Attribute("Type").set_value("Texture");
		assetData._Raw().append_attribute("TextureType");

		switch (type)
		{
		case aiTextureType::aiTextureType_DIFFUSE:
			assetData.Attribute("TextureType").set_value("Diffuse");
			material->TextureDiffuse = texture;
			break;
		case aiTextureType::aiTextureType_SPECULAR:
			assetData.Attribute("TextureType").set_value("Specular");
			material->TextureSpecular = texture;
			break;
		case aiTextureType::aiTextureType_HEIGHT:
			assetData.Attribute("TextureType").set_value("Normal");
			material->TextureNormals = texture;
			break;
		}

		if (!texture->Deserialize())
			SHADE_WARNING("Falied to import image :{0}", _path);

		texture->AssetInit();

		material->GetAssetData().Append(texture->GetAssetData());
	}
}

void IModel3D::ProcessMaterial(const shade::Shared<shade::Material3D>& material, const std::string& filePath, aiMaterial* aImaterial)
{
	aiColor3D color;
	//material->Get(AI_MATKEY_COLOR_AMBIENT, color); // Ka
	//mesh->GetMaterial()->ColorAmbient = glm::vec3(color.r, color.g, color.b);
	aImaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);// Kd
	material->ColorDiffuse = glm::vec3(color.r, color.g, color.b);
	aImaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);// Ks
	material->ColorSpecular = glm::vec3(color.r, color.g, color.b);
	aImaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, color);// Tf
	material->ColorTransparent = glm::vec3(color.r, color.g, color.b);

	float value;
	aImaterial->Get(AI_MATKEY_SHININESS, value); // Ns
	material->Shininess = value;

	auto& assetData = material->GetAssetData();
	assetData.Attribute("Id").set_value(aImaterial->GetName().C_Str());
	assetData.Attribute("Type").set_value("Material");
	//assetData._Raw().append_attribute("Asset");
	//assetData.Attribute("Asset").set_value(aImaterial->GetName().C_Str());
}

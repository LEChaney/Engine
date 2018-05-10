#include "ModelUtils.h"

#include "ModelComponent.h"
#include "Mesh.h"
#include "VertexFormat.h"
#include "Texture.h"
#include "GLUtils.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Log.h"
#include <unordered_map>

// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// The required info is returned as a Texture struct.
std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& textureDir)
{
	static std::unordered_map<std::string, Texture> s_texturesLoaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	std::vector<Texture> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		if (s_texturesLoaded.find(str.C_Str()) != s_texturesLoaded.end()) {
			textures.push_back(s_texturesLoaded.at(str.C_Str()));
			// A texture with the same filepath has already been loaded, continue to next one. (optimization)
		} else {
			// If texture hasn't been loaded already, load it
			Texture texture = GLUtils::loadTexture(textureDir + "/" + str.C_Str());
			s_texturesLoaded.insert(std::make_pair(str.C_Str(), texture));  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			textures.push_back(std::move(texture));
		}
	}
	return textures;
}

Material processMaterial(const aiMaterial* _aiMaterial, const aiScene* scene, const std::string& textureDir) 
{
	Material material;
	material.willDrawDepth = true;

	material.shaderParams.glossiness = 0.00001f;
	material.shaderParams.metallicness = 0.00001f;
	material.shaderParams.specBias = 0;

	// 1. diffuse maps
	std::vector<Texture> colorMaps = loadMaterialTextures(_aiMaterial, aiTextureType_DIFFUSE, textureDir);
	material.colorMaps.insert(material.colorMaps.end(), colorMaps.begin(), colorMaps.end());
	// 2. specular maps
	std::vector<Texture> metallicnessMaps = loadMaterialTextures(_aiMaterial, aiTextureType_SPECULAR, textureDir);
	material.metallicnessMaps.insert(material.metallicnessMaps.end(), metallicnessMaps.begin(), metallicnessMaps.end());
	// 3. gloss map
	std::vector<Texture> shininessMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_SHININESS, textureDir);
	material.shininessMaps.insert(material.shininessMaps.end(), shininessMaps.begin(), shininessMaps.end());
	// 4. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(_aiMaterial, aiTextureType_NORMALS, textureDir);
	material.normalMaps.insert(material.normalMaps.end(), normalMaps.begin(), normalMaps.end());
	// 5. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(_aiMaterial, aiTextureType_HEIGHT, textureDir);
	material.heightMaps.insert(material.heightMaps.end(), heightMaps.begin(), heightMaps.end());
	// 6. displacement maps
	std::vector<Texture> displacementMaps = loadMaterialTextures(_aiMaterial, aiTextureType_DISPLACEMENT, textureDir);
	material.displacementMaps.insert(material.displacementMaps.end(), displacementMaps.begin(), displacementMaps.end());

	// TODO: Add more of these to the material class
	//std::vector<Texture> ambientMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_AMBIENT, textureDir);
	//std::vector<Texture> emissiveMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_EMISSIVE, textureDir);
	//std::vector<Texture> lightMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_LIGHTMAP, textureDir);
	//std::vector<Texture> opacityMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_OPACITY, textureDir);
	//std::vector<Texture> reflectionMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_REFLECTION, textureDir);
	//std::vector<Texture> unknownTextureMaps = loadMaterialTextures(_aiMaterial, aiTextureType::aiTextureType_UNKNOWN, textureDir);

	// TODO: Swap shader model based on the textures we have
	if (colorMaps.size() == 0) 
	{
		aiColor3D color;
		_aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material.debugColor.r = color.r;
		material.debugColor.g = color.g;
		material.debugColor.b = color.b;
		material.shader = &GLUtils::getDebugShader();
	}
	else if (metallicnessMaps.size() > 0)
		material.shader = &GLUtils::getMetalShader();
	else
		material.shader = &GLUtils::getDefaultShader();

	return material;
}

Mesh processMesh(const aiMesh* _aiMesh, const aiScene* scene)
{
	bool hasNormals = _aiMesh->HasNormals();

	// Walk through each of the mesh's vertices
	std::vector<VertexFormat> vertices;
	for (GLuint i = 0; i < _aiMesh->mNumVertices; i++)
	{
		VertexFormat vertex;

		// Position
		vertex.position.x = _aiMesh->mVertices[i].x;
		vertex.position.y = _aiMesh->mVertices[i].y;
		vertex.position.z = _aiMesh->mVertices[i].z;

		// Normals
		if (hasNormals) {
			vertex.normal.x = _aiMesh->mNormals[i].x;
			vertex.normal.y = _aiMesh->mNormals[i].y;
			vertex.normal.z = _aiMesh->mNormals[i].z;
		}

		// Texture Coordinates
		if (_aiMesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = _aiMesh->mTextureCoords[0][i].x;
			vec.y = _aiMesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(std::move(vertex));
	}

	// Walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	std::vector<GLuint> indices;
	for (GLuint i = 0; i < _aiMesh->mNumFaces; i++)
	{
		aiFace& face = _aiMesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);

		// If the vertices have no normal information then calculate them from the face.
		// Assume flat shading and 3 vertices per faces.
		if (!hasNormals) {
			VertexFormat& triVert1 = vertices.at(face.mIndices[0]);
			VertexFormat& triVert2 = vertices.at(face.mIndices[1]);
			VertexFormat& triVert3 = vertices.at(face.mIndices[2]);
			glm::vec3 triEdge1 = triVert2.position - triVert1.position;
			glm::vec3 triEdge2 = triVert3.position - triVert2.position;
			glm::vec3 normal = glm::cross(triEdge1, triEdge2);
			triVert1.normal = normal;
			triVert2.normal = normal;
			triVert3.normal = normal;
		}
	}

	Mesh mesh;
	mesh.materialIndex = _aiMesh->mMaterialIndex;
	mesh.VAO = GLUtils::bufferMeshData(vertices, indices);
	mesh.numIndices = static_cast<GLsizei>(indices.size());

	// Return a mesh object created from the extracted mesh data
	return mesh;
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void processNode(const aiNode* _aiNode, const aiScene* scene, MeshNode& outNode) 
{
	// Link each mesh located at the current node
	for (GLuint i = 0; i < _aiNode->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		outNode.meshIDs.push_back(_aiNode->mMeshes[i]);
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < _aiNode->mNumChildren; i++)
	{
		outNode.childNodes.emplace_back();
		processNode(_aiNode->mChildren[i], scene, outNode.childNodes.at(i));
	}
}

ModelComponent ModelUtils::loadModel(const std::string& path)
{
	static std::unordered_map<std::string, ModelComponent> s_modelsLoaded;	// Stores all the models loaded so far, optimization to make sure models aren't loaded more than once.

	// A model with the same filepath has already been loaded, return a copy. (optimization)
	if (s_modelsLoaded.find(path) != s_modelsLoaded.end())
		return s_modelsLoaded.at(path);

	ModelComponent model;

	Assimp::Importer s_importer;
	const aiScene* scene = s_importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	// TODO: Throw an exception here
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		g_log << "ERROR::ASSIMP:: " << s_importer.GetErrorString() << "\n";
		return model;
	}
	std::string textureDir = path.substr(0, path.find_last_of('/'));

	// Load each mesh in the model
	for (GLuint i = 0; i < scene->mNumMeshes; ++i)
		model.meshes.push_back(processMesh(scene->mMeshes[i], scene));

	// Load each material in the model
	for (GLuint i = 0; i < scene->mNumMaterials; ++i)
		model.materials.push_back(processMaterial(scene->mMaterials[i], scene, textureDir));

	// Recursively construct the models scene graph hierachy
	processNode(scene->mRootNode, scene, model.rootNode);

	s_modelsLoaded.insert(std::make_pair(path, model));
	return model;
}

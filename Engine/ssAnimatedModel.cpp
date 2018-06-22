#include "ssAnimatedModel.h"

#include <stb_image.h>

#define POSITION_LOCATION	  0
#define NORMAL_LOCATION		  1
#define TEX_COORD_LOCATION	  2
#define BONE_ID_LOCATION	  3
#define BONE_WEIGHT_LOCATION  4 


void ssAnimatedModel::VertexBoneData::addBoneData(int BoneID, float Weight) {

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	//assert(0); //more bones than we have space for
}

ssAnimatedModel::ssAnimatedModel(std::string modelFilname, std::string texFilename, GLuint _program){

	//textureID = loadTexture(texFilename);
	program = _program;

	this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	this->position = glm::vec3(0.0, 0.0, 0.0);
	this->rotation = glm::vec3(1.0f, 0.0f, 0.0f);

	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_pScene = NULL;


	loadMesh(modelFilname);
}


ssAnimatedModel::~ssAnimatedModel(){

	Clear();
}

bool ssAnimatedModel::loadMesh(std::string fileName){
	
	Clear();

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	
	//create buffers for vertex attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool ret = false;

	m_pScene = m_Importer.ReadFile(fileName.c_str(), ASSIMP_LOAD_FLAGS);

	if (m_pScene) {		
		// load vertex, index, tex coord, material
		// bone and vertex data

		InitFromScene(m_pScene, fileName);
	}else{
		printf("Error parsing '%s': '%s'\n", fileName.c_str(), m_Importer.GetErrorString());
	}


	// load animation data
	printf(" ----------- bone hierarchy start-------- \n");

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	for (size_t i = 0; i < pAnimation->mNumChannels; i++) {
		
		aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		std::string nodeName = pNodeAnim->mNodeName.data;

		printf("ChannelName : %s \n", nodeName.c_str());

		// boneAiNodeMapping map -> maps nodeAnim to nodeName (same as bone name)
		// nodeAnim has rot, pos and scale info
		// it is with resect to parent node

		if (boneNameToAiNodeAnimMapping.find(nodeName) == boneNameToAiNodeAnimMapping.end()) {
			boneNameToAiNodeAnimMapping[nodeName] = pNodeAnim;
		}


	}

	printf(" ----------- bone hierarchy end -------- \n");


	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	//animation defaults
	startFrame = 31;
	endFrame = 50;
	animFps = m_pScene->mAnimations[0]->mChannels[0]->mNumPositionKeys / m_pScene->mAnimations[0]->mDuration;

	animStartTime = startFrame / float(animFps);
	animEndtime = endFrame / float(animFps);
	animationTime = animStartTime;
	animSpeed = 0.500f;

	animTick = 1.0f / float(animFps)  * animSpeed;

	return ret;
}

GLuint ssAnimatedModel::getVAO()
{
	return m_VAO;
}

GLuint ssAnimatedModel::getNumIndices()
{
	return m_numIndices;
}



bool ssAnimatedModel::InitFromScene(const aiScene * pScene, std::string Filename){
	
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<VertexBoneData> bones;
	std::vector<GLuint> indices;

	GLuint numVertices = 0;
	GLuint numIndices = 0;

	//count number of vertices and indices
	for (size_t i = 0; i < m_Entries.size(); i++) {
	
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = numVertices;
		m_Entries[i].BaseIndex = numIndices;

		numVertices += pScene->mMeshes[i]->mNumVertices;
		numIndices += m_Entries[i].NumIndices;
	}

	m_numIndices = numIndices;

	//reserve space in vectors for the vertex attributes and indices
	positions.reserve(numVertices);
	normals.reserve(numVertices);
	texcoords.reserve(numVertices);
	bones.resize(numVertices);
	indices.reserve(numIndices);

	// initialize the meshes in the scene one by one
	for (size_t i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		initMesh(i, paiMesh, positions, normals, texcoords, bones, indices);
	}

	if (!initMaterials(pScene, Filename)) {
		return false;
	}


	//generate and populate the buffers with vertex attributes and indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords[0]) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	return GLCheckError();
}

void ssAnimatedModel::initMesh(GLuint meshIndex,
								const aiMesh * paiMesh,
								std::vector<glm::vec3>& positions,
								std::vector<glm::vec3>& normals,
								std::vector<glm::vec2>& texcoords,
								std::vector<VertexBoneData>& bones,
								std::vector<GLuint>& indices){


	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	//populate the vertex attribute vectors
	for (size_t i = 0; i < paiMesh->mNumVertices; i++) {
	
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormals = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		normals.push_back(glm::vec3(pNormals->x, pNormals->y, pNormals->z));
		texcoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}


	//load bones 
	loadBones(meshIndex, paiMesh, bones);

	// populate the index buffer
	for (size_t i = 0; i < paiMesh->mNumFaces; i++) {
	
		const aiFace& face = paiMesh->mFaces[i];
		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

}

void ssAnimatedModel::loadBones(int meshIndex,
								const aiMesh * pMesh,
								std::vector<VertexBoneData>& bones){
									
	
	// For each bone 
	// In BoneTransformationInfo vector -> gets the offset matrix for each bone
	// In m_BoneNameToIDMapping map -> maps bone name to bone id
	// In VertexBoneData Vector -> stores per vertex which bone affects with what weightage

	for (int i = 0; i < pMesh->mNumBones; i++) { // for each bone
		
		int BoneIndex = 0;
		
		// get bone name for current bone index
		std::string BoneName(pMesh->mBones[i]->mName.data);

		//gets the initial offset matrix for each bone
		// stored in the m_boneinfo vector 
		if (m_BoneNameToIDMapping.find(BoneName) == m_BoneNameToIDMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones; // intitially 0
			m_NumBones++;

			// boneInfo stores offset and final transformation for bone
			BoneTransformationInfo bi;
			m_BoneTransformInfo.push_back(bi);

			// offset is relative to nodes parent
			// offset is the matrix relation between mesh and bone in bind pose

			// The mOffsetMatrix member is what's commonly referred to as the Inverse Bind Pose matrix
			// Transforming a bind-pose vertex by that matrix, 
			// Moves the vertex to the joint's local coordinate space.
			m_BoneTransformInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;// is the inverse bind pose matrix
			
			// map bone name to index
			// bonemapping maps bone id to bone name
			m_BoneNameToIDMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneNameToIDMapping[BoneName];
		}

		
		// Gets the bone index and bone weight of each bone affecting a vertex
		// stored in vertexBoneData (bone) -> stores per vertex bone data
		for (int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {

			int VertexID = m_Entries[meshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;

			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			
			// for each vertex 
			// get what bone affects it and with what weightage
			bones[VertexID].addBoneData(BoneIndex, Weight);
		}

	}// for each bone

}

bool ssAnimatedModel::initMaterials(const aiScene * pScene, const std::string filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (GLuint i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);

				if (p.substr(0, 2) == ".//") {
					p = p.substr(2, p.size() - 2);
				}

				std::string FullPath = Dir + "/" + p;

				GLuint TextureID = loadTexture(FullPath.c_str());
					m_Textures[i] = TextureID;

				if (m_Textures[i] == 0) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
				}else {
					printf("[Animated Model] %d - loaded texture '%s'\n", i, FullPath.c_str());
				}
				
			}
		}
	}

	return Ret;
}

GLuint ssAnimatedModel::loadTexture(std::string texFileName){
	
	GLuint mtexture;

	//** load texture
	glGenTextures(1, &mtexture);
	glBindTexture(GL_TEXTURE_2D, mtexture);

	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//** loadImage and create texture
	// Load image, create texture and generate mipmaps
	int width, height, numComponents;
	unsigned char* image = stbi_load(texFileName.c_str(), &width, &height, &numComponents, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	printf("\n [loadTexture] fileName %s \n", texFileName.c_str());

	return mtexture;

}


//movement
void ssAnimatedModel::move(float moveSpeed) {
	currentPlayerSpeed = moveSpeed;
}

void ssAnimatedModel::rotate(float rotSpeed) {

	currentRotationSpeed = rotSpeed;
}


//void ssAnimatedModel::setShaderEffectVariables(float dt, Terrain* terrain){
//	
//	glUseProgram(this->program);
//
//	//if(bIsTextureSet)
//	//glActiveTexture(GL_TEXTURE0);
//	//glBindTexture(GL_TEXTURE_2D, textureID);
//	//glUniform1i(glGetUniformLocation(program, "Texture"), 0);
//
//
//	glm::mat4 model;
//
//	rotation.y += currentRotationSpeed * .016f;
//
//	float distance = currentPlayerSpeed * .016f;
//
//	//printf("speed %f, \n", currentRotationSpeed);
//
//	float dx = (float)(distance * sin(glm::radians(rotation.y)));
//	float dz = (float)(distance * cos(glm::radians(rotation.y)));
//
//	this->position.x += dx;
//	this->position.z += dz;
//
//	this->position.y  = terrain->getHeight(this->position.x, this->position.z) - 0.0f;
//
//	glm::mat4 t = glm::translate(glm::mat4(), this->position);
//	glm::mat4 r = glm::rotate(glm::mat4(), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
//	glm::mat4 s = glm::scale(glm::mat4(), this->scale);
//	
//	model = t * r * s;
//
//	glm::mat4 vp = camera->getprojectionMatrix() * camera->getViewMatrix();
//	GLint vpLoc = glGetUniformLocation(program, "vp");
//	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));
//
//	GLint modelLoc = glGetUniformLocation(program, "model");
//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
//
//
//	// lighting calculations
//	GLint colorLoc = glGetUniformLocation(program, "objectColor");
//	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);
//
//	GLuint cameraPosLoc = glGetUniformLocation(program, "cameraPos");
//	glUniform3f(cameraPosLoc, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);
//
//	GLuint lightPosLoc = glGetUniformLocation(program, "lightPos");
//	glUniform3f(lightPosLoc, this->light->getPosition().x, this->light->getPosition().y, this->light->getPosition().z);
//
//	GLuint lightColorLoc = glGetUniformLocation(program, "lightColor");
//	glUniform3f(lightColorLoc, this->light->getColor().x, this->light->getColor().y, this->light->getColor().z);
//
//	GLuint specularStrengthLoc = glGetUniformLocation(program, "specularStrength");
//	glUniform1f(specularStrengthLoc, 0.1f);
//
//	GLuint ambientStrengthLoc = glGetUniformLocation(program, "ambientStrength");
//	glUniform1f(ambientStrengthLoc, 0.5f);
//
//
//	// get uniform location for transforms
//	for (unsigned int i = 0; i < ARRAY_SIZE(m_boneLocation); i++) {
//		char name[128];
//		memset(name, 0, sizeof(name));
//		sprintf_s(name, "jointTransforms[%d]", i);
//		m_boneLocation[i] = glGetUniformLocation(program, name);
//	}
//
//	std::vector<Matrix4f> transforms; // = getJointTransforms();
//
//	boneTransforms(dt, transforms);
//
//	for (int i = 0; i < transforms.size(); i++) {
//		Matrix4f Transform = transforms[i];
//		glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)(Transform));
//	}
//}


//void ssAnimatedModel::render(float dt, Terrain* terrain){
//	
//	setShaderEffectVariables(dt, terrain);
//
//	glBindVertexArray(m_VAO);
//
//	for (GLuint i = 0; i < m_Entries.size(); i++) {
//		
//		GLuint MaterialIndex = m_Entries[i].MaterialIndex;
//
//		assert(MaterialIndex < m_Textures.size());
//
//		if (m_Textures[MaterialIndex]) {
//			//m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
//			//glBindTexture(GL_TEXTURE_2D, m_Textures[MaterialIndex]);
//						
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, m_Textures[MaterialIndex]);
//			//glUniform1i(glGetUniformLocation(program, "Texture"), MaterialIndex);
//		}
//
//		//glDrawElementsBaseVertex(GL_TRIANGLES,
//		//						m_Entries[i].NumIndices, 
//		//						GL_UNSIGNED_INT,
//		//						(void*)(sizeof(GLuint) * m_Entries[i].BaseIndex),
//		//						m_Entries[i].BaseVertex);
//
//		glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
//	}
//
//	// Make sure the VAO is not changed from the outside    
//	glBindVertexArray(0);
//
//}




void ssAnimatedModel::boneTransforms(float timeInSeconds, std::vector<Matrix4f>& transforms){
			

		//float animDuration = (float)m_pScene->mAnimations[0]->mDuration;
		//float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		//float TimeInTicks = (timeInSeconds) * TicksPerSecond;
		//float AnimationTime = fmod(TimeInTicks, animDuration);

		animationTime += animTick;

		if (animationTime >= animEndtime) {
			animationTime = animStartTime;
		}

		// Go through the bone hierachy
		// And set the final transformation for each bone
		// Starting from the root node
		Matrix4f Identity; Identity.InitIdentity();
		ReadNodeHeirarchy(animationTime, m_pScene->mRootNode, Identity);

		transforms.resize(m_NumBones);

		// set the trasnformation matrix for each bone
		for (GLuint i = 0; i < m_NumBones; i++) {
			transforms[i] = m_BoneTransformInfo[i].FinalTransformation;
		}

}

void ssAnimatedModel::setCurrentAnimation(int startFrameNum, int endFramNum){
	
	animStartTime = startFrameNum / float(animFps);
	animEndtime = endFramNum / float(animFps);

	animationTime = animStartTime;
}

//////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////

void ssAnimatedModel::ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, const Matrix4f & ParentTransform) {

	// initially pNode is the root node
	// and parent transform is identity

	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];
	
	//transformation relative to nodes parent
	Matrix4f NodeTransformation(pNode->mTransformation); 
	
	//Can initialize with identity but then model is 10 times bigger
	//NodeTransformation.InitIdentity(); 
	
	// returns aiNodeAnim, for a bone name 
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	// If node has animations
	if (pNodeAnim) {
		

		// Get position, scale and rotation
		// All are w.r.t parent bone

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

		Matrix4f TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
		//glm::mat4 TranslationM = glm::translate(glm::mat4(), glm::vec3(Translation.x, Translation.y, Translation.z));
		
		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		
		Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
		//glm::quat myQuat = glm::quat(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w);
		//glm::mat4 RotationM = glm::toMat4(myQuat);

		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		
		Matrix4f ScalingM; ScalingM.InitIdentity();
		ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
		//glm::mat4 ScalingM = glm::scale(glm::mat4(), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		
		// Combine the above transformations
		// and store in nodeTransform
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	
	Matrix4f modalAnimParentTransform = ParentTransform * //modalSpace parentBone animTransform
										NodeTransformation; //boneSpace currentbone animTransform

	if (m_BoneNameToIDMapping.find(NodeName) != m_BoneNameToIDMapping.end()) {
		
		// get ID of current bone name
		size_t BoneIndex = m_BoneNameToIDMapping[NodeName];

		// Global transform
		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation; // scene space

		// inverse Global transform
		m_GlobalInverseTransform.Inverse();  //scene inverse space

		//calculate final bone transformation info
		m_BoneTransformInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * // sceneSpace transform 
															 modalAnimParentTransform *  // modelspace anim transform for current bone
															 m_BoneTransformInfo[BoneIndex].BoneOffset;  // Inverse Bind Pose matrix - transforms from bind pose to local
	}


	// Do the same for all the children in the hierarchy
	// By calling itself

	for (size_t i = 0; i < pNode->mNumChildren; i++) {

		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], modalAnimParentTransform);

	}
}

const aiNodeAnim * ssAnimatedModel::FindNodeAnim(const aiAnimation* pAnimation, std::string NodeName) {

	/*
	for (size_t i = 0; i < pAnimation->mNumChannels; i++) {

		// a channel is a bone with all tranformation information
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}
	*/

	//optimized implementation of above code
	if (boneNameToAiNodeAnimMapping.find(NodeName) != boneNameToAiNodeAnimMapping.end()) {
		const aiNodeAnim* pNodeAnim = boneNameToAiNodeAnimMapping[NodeName];
		return pNodeAnim;
	}

	return NULL;
}

void ssAnimatedModel::CalcInterpolatedPosition(aiVector3D & out, float AnimationTime, const aiNodeAnim * pNodeAnim){
	
	if (pNodeAnim->mNumPositionKeys == 1) {
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	size_t positionIndex = FindPosition(AnimationTime, pNodeAnim);
	size_t NextPositionIndex = (positionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);

	float deltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
	float factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = pNodeAnim->mPositionKeys[positionIndex].mValue;
	const aiVector3D& end = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D delta = end - start;

	out = start + factor * delta;

}

GLuint ssAnimatedModel::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	for (size_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}

void ssAnimatedModel::CalcInterpolatedRotation(aiQuaternion & out, float AnimationTime, const aiNodeAnim * pNodeAnim){

	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	size_t RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	size_t NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();
}


GLuint ssAnimatedModel::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	assert(pNodeAnim->mRotationKeys > 0);

	for (size_t i = 0; pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}

void ssAnimatedModel::CalcInterpolatedScaling(aiVector3D & out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		size_t ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		size_t NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		out = Start + Factor * Delta;

	}
}


GLuint ssAnimatedModel::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	assert(pNodeAnim->mNumScalingKeys > 0);

	for (size_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}


void ssAnimatedModel::Clear() {


	for (size_t i = 0; i < m_Textures.size(); i++) {
		//SAFE_DELETE(m_Textures[i]);
	}

	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}



///////////////// getter and setters //////////////////////

void ssAnimatedModel::setPosition(glm::vec3 _position) {
	this->position = _position;
}

void ssAnimatedModel::setRotation(glm::vec3 _rotation) {
	this->rotation = _rotation;
}

void ssAnimatedModel::setScale(glm::vec3 _scale) {
	this->scale = _scale;
}

void ssAnimatedModel::setSpeed(float _speed) {
	//this->playerSpeed = _speed;
}


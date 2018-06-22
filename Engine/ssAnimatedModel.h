#pragma once


// https://realitymultiplied.wordpress.com/2016/07/23/assimp-skeletal-animation-tutorial-2-loading-up-the-bone-data/
// https://github.com/SaschaWillems/Vulkan/blob/master/skeletalanimation/skeletalanimation.cpp
// http://techqa.info/programming/tag/assimp?after=38129536
// https://github.com/akafonzie/OpenGLAssimpModelLoader/blob/master/modelLoader.cpp

//aiMath to glm conversion
// https://github.com/mackron/GTGameEngine/blob/master/source/ModelDefinition_Assimp.cpp

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


#include <iostream>
#include <fstream>

#include <vector>
#include <map>

#include <fstream>

//#include "Math_3d.h"

#include "Terrain.h"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define GLCheckError() (glGetError() == GL_NO_ERROR)


#define INVALID_MATERIAL 0xFFFFFFFF


class Matrix4f
{
public:
    float m[4][4];

    Matrix4f(){ }

    // constructor from Assimp matrix
    Matrix4f(const aiMatrix4x4& AssimpMatrix) {
        m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
        m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
        m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
        m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
    }

    Matrix4f(const aiMatrix3x3& AssimpMatrix) {
        m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
        m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
        m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
        m[3][0] = 0.0f           ; m[3][1] = 0.0f           ; m[3][2] = 0.0f           ; m[3][3] = 1.0f;
    }

    void SetZero(){
        //ZERO_MEM(m);
		memset(m, 0, sizeof(m));
    }
	
    inline void InitIdentity()   {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    inline Matrix4f operator*(const Matrix4f& Right) const {
        Matrix4f Ret;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                              m[i][1] * Right.m[1][j] +
                              m[i][2] * Right.m[2][j] +
                              m[i][3] * Right.m[3][j];
            }
        }

        return Ret;
    }



    operator const float*() const {
        return &(m[0][0]);
    }


	float Determinant() const {
		return m[0][0] * m[1][1] * m[2][2] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] + m[0][0] * m[1][2] * m[2][3] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3]
			+ m[0][0] * m[1][3] * m[2][1] * m[3][2] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][1] * m[1][2] * m[2][3] * m[3][0] + m[0][1] * m[1][2] * m[2][0] * m[3][3]
			- m[0][1] * m[1][3] * m[2][0] * m[3][2] + m[0][1] * m[1][3] * m[2][2] * m[3][0] - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2]
			+ m[0][2] * m[1][3] * m[2][0] * m[3][1] - m[0][2] * m[1][3] * m[2][1] * m[3][0] + m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1]
			+ m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][3] * m[1][0] * m[2][2] * m[3][1]
			- m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][3] * m[1][2] * m[2][1] * m[3][0];
	}


	Matrix4f& Inverse() 	{
		// Compute the reciprocal determinant
		float det = Determinant();
		if (det == 0.0f){
			// Matrix not invertible. 
			assert(0);
			return *this;
		}

		float invdet = 1.0f / det;

		Matrix4f res;
		res.m[0][0] = invdet  * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
		res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
		res.m[0][2] = invdet  * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3]) + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
		res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
		res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
		res.m[1][1] = invdet  * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
		res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
		res.m[1][3] = invdet  * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
		res.m[2][0] = invdet  * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[2][2] = invdet  * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) + m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
		res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) + m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
		res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[3][1] = invdet  * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
		res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) + m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2]) + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
		res.m[3][3] = invdet  * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
		*this = res;

		return *this;
	}


	void InitTranslationTransform(float x, float y, float z) {

		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ) {
		m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
		m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
		m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
		m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
	}

};


struct MeshEntry {

	MeshEntry() {

		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		MaterialIndex = INVALID_MATERIAL;

	}

	GLuint NumIndices;
	GLuint BaseVertex;
	GLuint BaseIndex;
	GLuint MaterialIndex;
};

enum VB_TYPES {
	INDEX_BUFFER,
	POS_VB,
	NORMAL_VB,
	TEXCOORD_VB,
	BONE_VB,
	NUM_VBs
};


class ssAnimatedModel
{
public:
	ssAnimatedModel(std::string modelFilname,
					std::string texFilename,
							GLuint _program);
	~ssAnimatedModel();


#define NUM_BONES_PER_VERTEX 4

	//stores offset matrix and final transforms matrix for each bone
	struct BoneTransformationInfo {

		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;

		BoneTransformationInfo() {
			BoneOffset.SetZero();// = glm::mat4(); //-->setzero
			FinalTransformation.SetZero();  // = glm::mat4();  //--> setzero
		}
	};

	//ids and bones affecting each vertex
	struct VertexBoneData {

		int IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		VertexBoneData() {
			reset();
		}

		void reset() {
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		//int boneID; 
		//float weight;
		void addBoneData(int BoneID, float Weight);
	};

	bool loadMesh(std::string fileName);

	GLuint getVAO();

	GLuint getNumIndices();

	GLuint numBones() {
		return mNumBones;
	}

	void boneTransforms(float timeInSeconds, std::vector<Matrix4f> &transforms);
	void setCurrentAnimation(int startFrameNum, int endFramNum);

	void setPosition(glm::vec3 _position);
	void setRotation(glm::vec3 _rotation);
	void setScale(glm::vec3 _scale);
	void setSpeed(float _speed);

	void move(float speed);
	void rotate(float speed);
	
	bool bMoving = false;
	bool bFalling = false;

private:

	GLuint program;

	int mNumBones;

	bool InitFromScene(const aiScene* pScene, const std::string Filename);

	void initMesh(GLuint meshIndex,
		const aiMesh*paiMesh,
		std::vector<glm::vec3>& positions,
		std::vector<glm::vec3>& normals,
		std::vector<glm::vec2>&texcoords,
		std::vector<VertexBoneData>&bones,
		std::vector<GLuint>&indices);

	void loadBones(int meshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& bones);
	bool initMaterials(const aiScene* pScene, const std::string filename);
	GLuint loadTexture(std::string  texFileName);
	//void setShaderEffectVariables(float dt, Terrain* terrain);
	void Clear();

	//rendering
	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];
	GLuint m_numIndices;
	
	std::vector<MeshEntry> m_Entries;
	std::vector<GLuint> m_Textures;

	std::map<std::string, GLuint> m_BoneNameToIDMapping;
	std::map<std::string, aiNodeAnim*> boneNameToAiNodeAnimMapping;
	std::vector<BoneTransformationInfo> m_BoneTransformInfo;
	GLuint m_NumBones;

	Matrix4f m_GlobalInverseTransform;
	GLuint m_boneLocation[100];

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;

	GLuint numMaterialsCounter;
	
	//animation variables
	float animTick;
	int startFrame;
	int endFrame;
	int currentFrame;
	float animSpeed;

	int animFps;
	float animStartTime;
	float animEndtime;
	
	float animationTime;

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	float currentPlayerSpeed;
	float currentRotationSpeed;

	// Helper functions 
	// Data conversion and nodefinder 
	GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void CalcInterpolatedPosition(aiVector3D& out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, std::string NodeName);

	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);

};


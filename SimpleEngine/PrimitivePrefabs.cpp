#define _USE_MATH_DEFINES

#include "PrimitivePrefabs.h"

#include "GLUtils.h"
#include "GLPrimitives.h"
#include "Scene.h"
#include "Entity.h"
#include "ModelUtils.h"
#include "Log.h"
#include "stb_image.h"
#include "VertexFormat.h"
#include "Utils.h"

#include <glm\gtc\matrix_transform.hpp>

#include <memory>
#include <array>

using namespace glm;

namespace Prefabs {

	Entity& createQuad(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;


		entity.model = GLPrimitives::getQuadModel();
		entity.model.materials.at(0).shaderParams.glossiness = 0.0f;
		entity.model.materials.at(0).shaderParams.metallicness = 0.0f;
		entity.model.materials.at(0).shaderParams.specBias = 0;

		// Replace default texture
		entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/dessert-floor.png");

		return entity;
	}

	Entity& createSphere(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;

		entity.model = GLPrimitives::getSphereModel();

		return entity;
	}

	Entity& createCylinder(Scene& scene, float radius, float height, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;
		entity.transform.scale.x *= radius;
		entity.transform.scale.y *= height;
		entity.transform.scale.z *= radius;

		entity.model = GLPrimitives::getCylinderModel();

		return entity;
	}

	Entity& createPyramid(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;

		entity.model = GLPrimitives::getPyramidModel();

		return entity;
	}

	Entity& createCube(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;

		entity.model = GLPrimitives::getCubeModel();

		return entity;
	}

	Entity& createCamera(Scene& scene, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
	{
		Entity& entity = scene.createEntity(COMPONENT_CAMERA, COMPONENT_TRANSFORM);

		entity.camera.setLookAt(pos, center, up);

		return entity;
	}

	Entity& createSkybox(Scene& scene, const std::vector<std::string>& faceFilenames)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL);

		entity.model = GLPrimitives::getCubeModel();

		// Replace default material
		entity.model.materials.at(0) = {};
		entity.model.materials.at(0).shader = &GLUtils::getSkyboxShader();
		entity.model.materials.at(0).colorMaps.push_back(GLUtils::loadCubeMapFaces(faceFilenames));
		entity.model.materials.at(0).willDrawDepth = false;

		return entity;
	}

	Entity& createModel(Scene& scene, const std::string& path, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.model = ModelUtils::loadModel(path);

		entity.transform = transform;

		return entity;
	}
}
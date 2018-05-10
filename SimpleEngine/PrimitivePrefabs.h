//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of functions used to create new entities.
// Author       : Lance Chaney, Jack Mair
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "TransformComponent.h"

#include <glm\glm.hpp>
#include <glad\glad.h>

#include <vector>
#include <string>
#include <cstdint>

class Scene;
class Entity;
struct VertexFormat;
struct Mesh;
struct InputComponent;
struct InputMapComponent;

namespace Prefabs {
	// Creates a unit square facing down the positive z axis with the 
	// specified lookAt
	Entity& createQuad(Scene&, const TransformComponent& transform = {});

	// Creates a unit sphere, centered at the origin, with the specified 
	// lookAt;
	Entity& createSphere(Scene&, const TransformComponent& transform = {});

	// Creates a cylinder with the specified radius and height.
	Entity& createCylinder(Scene&, float radius = 1, float height = 1, const TransformComponent& transform = {});

	// Creates a pyramid
	Entity& createPyramid(Scene&, const TransformComponent& transform = {});

	// Creates a pyramid
	Entity& createCube(Scene&, const TransformComponent& transform = {});

	// Creates a camera.
	// This camera needs to be set as active on the render in order to be rendered from.
	Entity& createCamera(Scene&, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up = glm::vec3{ 0, 1, 0 });

	// Creates a skybox.
	// Can be used to set the environment map for the renderer.
	Entity& createSkybox(Scene&, const std::vector<std::string>& faceFilenames);

	// Creates an entity from a 3D model file.
	// The entity returned is a simple entity with only a model and a lookAt component.
	Entity& createModel(Scene&, const std::string& path, const TransformComponent& transform = {});
}
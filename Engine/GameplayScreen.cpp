#define _USE_MATH_DEFINES
#include "GameplayScreen.h"

#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "PrimitivePrefabs.h"
#include "GLUtils.h"
#include "LevelLoader.h"
#include "BasicCameraMovementSystem.h"
#include "Utils.h"
#include "Terrain.h"
#include "TerrainFollowSystem.h"
#include "SimpleWorldSpaceMoveSystem.h"
#include "ClothSystem.h"
#include "MousePickingSystem.h"
#include "CollisionSystem.h"
#include "ModelUtils.h"

#include <glm\gtc\constants.hpp>
#include <nanogui\nanogui.h>

#include <cmath>
#include <list>

enum test_enum {
	Item1 = 0,
	Item2,
	Item3
};

GameplayScreen::GameplayScreen()
	: Screen()
	, m_clothPointsX(10)
	, m_clothPointsY(10)
	, m_maxClothPoints(20)
	, m_hookDistance(1)
	, m_clothSize(2.0f)
{
	setupGui();

	// Init systems
	m_activeSystems.push_back(std::make_unique<InputSystem>(m_scene));
	m_activeSystems.push_back(std::make_unique<TerrainFollowSystem>(m_scene));
	m_activeSystems.push_back(std::make_unique<SimpleWorldSpaceMoveSystem>(m_scene));
	auto basicCameraMovementSystem = std::make_unique<BasicCameraMovementSystem>(m_scene);
	auto renderSystem = std::make_unique<RenderSystem>(m_scene);

	// Create environment map / skybox
	Entity& skybox = Prefabs::createSkybox(m_scene, {
		"Assets/Textures/envmap_violentdays/violentdays_rt.tga",
		"Assets/Textures/envmap_violentdays/violentdays_lf.tga",
		"Assets/Textures/envmap_violentdays/violentdays_up.tga",
		"Assets/Textures/envmap_violentdays/violentdays_dn.tga",
		"Assets/Textures/envmap_violentdays/violentdays_bk.tga",
		"Assets/Textures/envmap_violentdays/violentdays_ft.tga",
	});
	Texture irradianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_iem.dds");
	Texture radianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_pmrem.dds");
	renderSystem->setRadianceMap(radianceMap.id);
	renderSystem->setIrradianceMap(irradianceMap.id);

	// Setup the camera
	Entity& cameraEntity = Prefabs::createCamera(m_scene, { 0, 0, 3 }, { 0, -1, 0 }, { 0, 1, 0 });
	renderSystem->setCamera(&cameraEntity);
	basicCameraMovementSystem->setCameraToControl(&cameraEntity);

	//Prefabs::createTerrain(m_scene, "Assets/Textures/Heightmaps/heightmap_2.png", 100, 100);
	//Entity& terrain = Prefabs::createTerrain(m_scene, "Assets/Textures/Heightmaps/heightmap_2.png", 1000);

	//Entity& reflectiveSphere = Prefabs::createSphere(m_scene);
	//reflectiveSphere.transform.position += glm::vec3(0, 40, 0);
	//reflectiveSphere.model.materials[0].shader = &GLUtils::getDebugShader();
	//reflectiveSphere.model.materials[0].debugColor = glm::vec3(1, 1, 1);
	//reflectiveSphere.model.materials[0].shaderParams.glossiness = 1.0f;
	//reflectiveSphere.model.materials[0].shaderParams.metallicness = 1.0f;
	//reflectiveSphere.addComponents(COMPONENT_TERRAIN_FOLLOW, COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT,
	//                               COMPONENT_INPUT, COMPONENT_INPUT_MAP);
	//reflectiveSphere.terrainFollow.terrainToFollow = &terrain;
	//reflectiveSphere.inputMap.forwardBtnMap = GLFW_KEY_UP;
	//reflectiveSphere.inputMap.backwardBtnMap = GLFW_KEY_DOWN;
	//reflectiveSphere.inputMap.leftBtnMap = GLFW_KEY_LEFT;
	//reflectiveSphere.inputMap.rightBtnMap = GLFW_KEY_RIGHT;
	//reflectiveSphere.simpleWorldSpaceMovement.moveSpeed = 10;
	//reflectiveSphere.terrainFollow.followerHalfHeight = 1.0f;

	//Entity& diffuseSphere = Prefabs::createSphere(m_scene);
	//diffuseSphere.transform.position += glm::vec3(5, 40, 0);
	//diffuseSphere.model.materials[0].shader = &GLUtils::getDebugShader();
	//diffuseSphere.model.materials[0].debugColor = glm::vec3(1, 1, 1);
	//diffuseSphere.model.materials[0].shaderParams.glossiness = 0.0f;
	//diffuseSphere.model.materials[0].shaderParams.metallicness = 0.0f;
	//diffuseSphere.model.materials[0].shaderParams.specBias = -0.04f;
	//diffuseSphere.addComponents(COMPONENT_TERRAIN_FOLLOW, COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT,
	//	COMPONENT_INPUT, COMPONENT_INPUT_MAP);
	//diffuseSphere.terrainFollow.terrainToFollow = &terrain;
	//diffuseSphere.inputMap.forwardBtnMap = GLFW_KEY_UP;
	//diffuseSphere.inputMap.backwardBtnMap = GLFW_KEY_DOWN;
	//diffuseSphere.inputMap.leftBtnMap = GLFW_KEY_LEFT;
	//diffuseSphere.inputMap.rightBtnMap = GLFW_KEY_RIGHT;
	//diffuseSphere.simpleWorldSpaceMovement.moveSpeed = 10;
	//diffuseSphere.terrainFollow.followerHalfHeight = 1.0f;

	TransformComponent sphereTransform;
	sphereTransform.position.y = -1.2f;
	sphereTransform.position.x = 0.0f;
	sphereTransform.position.z = -2.0f;
	Entity& sphere = Prefabs::createSphere(m_scene, sphereTransform);
	sphere.addComponents(COMPONENT_SPHERE_COLLISION | COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT | COMPONENT_INPUT | COMPONENT_INPUT_MAP);
	sphere.sphereCollision.radius = 1.0f;
	sphere.inputMap.backwardBtnMap = GLFW_KEY_DOWN;
	sphere.inputMap.forwardBtnMap = GLFW_KEY_UP;
	sphere.inputMap.leftBtnMap = GLFW_KEY_LEFT;
	sphere.inputMap.rightBtnMap = GLFW_KEY_RIGHT;
	sphere.simpleWorldSpaceMovement.moveSpeed = 1.0f;

	TransformComponent triangleTransform;
	triangleTransform.position.y = -1.4f;
	triangleTransform.position.x = -5.0f;
	triangleTransform.position.z = -2.0f;
	Entity& pyramid = Prefabs::createPyramid(m_scene, triangleTransform);
	pyramid.addComponents(COMPONENT_PYRAMID_COLLISION | COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT | COMPONENT_INPUT | COMPONENT_INPUT_MAP);
	pyramid.sphereCollision.radius = 1.0f;
	pyramid.inputMap.backwardBtnMap = GLFW_KEY_DOWN;
	pyramid.inputMap.forwardBtnMap = GLFW_KEY_UP;
	pyramid.inputMap.leftBtnMap = GLFW_KEY_LEFT;
	pyramid.inputMap.rightBtnMap = GLFW_KEY_RIGHT;
	pyramid.simpleWorldSpaceMovement.moveSpeed = 1.0f;

	TransformComponent capsuleTransform;
	capsuleTransform.position.y = -1.8f;
	capsuleTransform.position.x = 5.0f;
	capsuleTransform.position.z = -2.0f;
	capsuleTransform.eulerAngles = { 0.0f, 0.0f, -glm::quarter_pi<float>() };
	Entity& capsule = m_scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);
	capsule.transform = capsuleTransform;
	capsule.model = ModelUtils::loadModel("Assets/Models/capsule/Capsule.obj");
	capsule.addComponents(COMPONENT_CAPSULE_COLLISION | COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT | COMPONENT_INPUT | COMPONENT_INPUT_MAP);
	capsule.inputMap.backwardBtnMap = GLFW_KEY_DOWN;
	capsule.inputMap.forwardBtnMap = GLFW_KEY_UP;
	capsule.inputMap.leftBtnMap = GLFW_KEY_LEFT;
	capsule.inputMap.rightBtnMap = GLFW_KEY_RIGHT;
	capsule.simpleWorldSpaceMovement.moveSpeed = 1.0f;

	TransformComponent groundTransform;
	groundTransform.position.y = -3.0f;
	groundTransform.position.x = 0.0f;
	groundTransform.eulerAngles = {-glm::half_pi<float>(), 0.0f, 0.0f};
	groundTransform.scale = { 100.0f, 100.0f, 1.0f };
	Entity& ground = Prefabs::createQuad(m_scene, groundTransform);
	ground.addComponents(COMPONENT_GROUND_COLLISION);

	Entity& cloth = Prefabs::createCloth(m_scene, m_clothPointsX, m_clothPointsY, m_clothSize, m_clothSize, 1, m_hookDistance);
	cloth.cloth.clothPoints = m_clothPointsX;
	cloth.cloth.maxClothPoints = m_maxClothPoints;

	//m_activeSystems.push_back(std::move(basicCameraMovementSystem));
	m_activeSystems.push_back(std::move(renderSystem));
	m_activeSystems.push_back(std::make_unique<MousePickingSystem>(m_scene, cameraEntity));
	m_activeSystems.push_back(std::make_unique<PhysicsSystem>(m_scene));
	m_activeSystems.push_back(std::make_unique<SimpleWorldSpaceMoveSystem>(m_scene));
	m_activeSystems.push_back(std::make_unique<CollisionSystem>(m_scene));
	auto clothSystem = std::make_unique<ClothSystem>(m_scene);
	m_clothSystem = clothSystem.get();
	m_activeSystems.push_back(std::move(clothSystem));
}


GameplayScreen::~GameplayScreen()
{
}

// TODO: divide this into functions
void GameplayScreen::setupGui()
{
	

	window = new nanogui::Window(m_uiScreen, "Options");
	window->setPosition(nanogui::Vector2i(15, 15));
	window->setLayout(new nanogui::GroupLayout());

	nanogui::Button *resetButton = new nanogui::Button(window, "Reset");
	// On button press
	resetButton->setCallback([&] {
		std::cout << "pushed!" << std::endl;
		for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
			Entity& clothEntity = m_scene.getEntity(i);
			if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
				m_scene.destroyEntity(clothEntity);
				Entity& newCloth = Prefabs::createCloth(m_scene, m_clothPointsX, m_clothPointsY, m_clothSize, m_clothSize, 1, m_hookDistance);
				newCloth.cloth.clothPoints = m_clothPointsX;
				newCloth.cloth.maxClothPoints = m_maxClothPoints;
				m_foldSlider->setValue(1.0f);
				m_foldTextBox->setValue("2");
			}
		}
	});
	resetButton->setTooltip("resets cloth");

	// Detail slider
	new nanogui::Label(window, "Detail", "sans-bold");

	nanogui::Widget *detailPanel = new nanogui::Widget(window);
	detailPanel->setLayout(new nanogui::BoxLayout(
		nanogui::Orientation::Horizontal,
		nanogui::Alignment::Middle, 0, 20));

	nanogui::Slider *detailSlider = new nanogui::Slider(detailPanel);
	detailSlider->setValue(0.0f);
	detailSlider->setFixedWidth(180);

	nanogui::TextBox *detailTextBox = new nanogui::TextBox(detailPanel);
	detailTextBox->setFixedSize(nanogui::Vector2i(60, 25));
	detailTextBox->setValue("10");
	detailTextBox->setUnits("pts");
	// On slider move
	detailSlider->setCallback([&, detailTextBox](float value) {
		detailTextBox->setValue(std::to_string((int)(value * 6) + 10));
		m_clothPointsX = (int)(value * 6) + 10;
		m_clothPointsY = (int)(value * 6) + 10;
		for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
			Entity& clothEntity = m_scene.getEntity(i);
			if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
				m_scene.destroyEntity(clothEntity);
				Entity& newCloth = Prefabs::createCloth(m_scene, m_clothPointsX, m_clothPointsY, m_clothSize, m_clothSize, 1, m_hookDistance);
				newCloth.cloth.clothPoints = m_clothPointsX;
				newCloth.cloth.maxClothPoints = m_maxClothPoints;
				m_foldSlider->setValue(1.0f);
				m_foldTextBox->setValue("2");
			}
		}
	});
	// On slider release
	detailSlider->setFinalCallback([&](float value) {
		std::cout << "Final slider value: " << (int)(value * 6) + 10 << std::endl;
	});
	detailTextBox->setFontSize(20);
	detailTextBox->setAlignment(nanogui::TextBox::Alignment::Right);

	// Hook slider
	new nanogui::Label(window, "Hook distance", "sans-bold");

	nanogui::Widget *hookPanel = new nanogui::Widget(window);
	hookPanel->setLayout(new nanogui::BoxLayout(
		nanogui::Orientation::Horizontal,
		nanogui::Alignment::Middle, 0, 20));

	nanogui::Slider *hookSlider = new nanogui::Slider(hookPanel);
	hookSlider->setValue(0.0f);
	hookSlider->setFixedWidth(180);

	nanogui::TextBox *hookTextBox = new nanogui::TextBox(hookPanel);
	hookTextBox->setValue("1");
	hookTextBox->setUnits("");
	// On slider move
	hookSlider->setCallback([&, hookTextBox](float value) {
		hookTextBox->setValue(std::to_string((int)(value * 9) + 1));
		m_hookDistance = (int)(value * 9) + 1;
		for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
			Entity& clothEntity = m_scene.getEntity(i);
			if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
				m_scene.destroyEntity(clothEntity);
				Entity& newCloth = Prefabs::createCloth(m_scene, m_clothPointsX, m_clothPointsY, m_clothSize, m_clothSize, 1, m_hookDistance);
				newCloth.cloth.clothPoints = m_clothPointsX;
				newCloth.cloth.maxClothPoints = m_maxClothPoints;
				m_foldSlider->setValue(1.0f);
				m_foldTextBox->setValue("2");
			}
		}
	});
	// On slider release
	hookSlider->setFinalCallback([&](float value) {
		std::cout << "Final slider value: " << (int)(value * 9) + 1 << std::endl;
	});
	hookTextBox->setFixedSize(nanogui::Vector2i(60, 25));
	hookTextBox->setFontSize(20);
	hookTextBox->setAlignment(nanogui::TextBox::Alignment::Right);

	// Wind direction slider
	new nanogui::Label(window, "Wind angle", "sans-bold");

	nanogui::Widget *windAnglePanel = new nanogui::Widget(window);
	windAnglePanel->setLayout(new nanogui::BoxLayout(
		nanogui::Orientation::Horizontal,
		nanogui::Alignment::Middle, 0, 20));

	nanogui::Slider *windAngleSlider = new nanogui::Slider(windAnglePanel);
	windAngleSlider->setValue(0.0f);
	windAngleSlider->setFixedWidth(180);

	nanogui::TextBox *windDirectionTextBox = new nanogui::TextBox(windAnglePanel);
	windDirectionTextBox->setValue("0");
	windDirectionTextBox->setUnits("deg");
	
	// On slider move
	windAngleSlider->setCallback([&, windDirectionTextBox](float value) {
		windDirectionTextBox->setValue(std::to_string((int)(value * 360)));
		float angle = glm::radians(value * 360.0f);
		m_clothSystem->windDirection.x = glm::cos(angle);
		m_clothSystem->windDirection.z = -glm::sin(angle);
	});
	// On slider release
	windAngleSlider->setFinalCallback([&](float value) {
		std::cout << "Final slider value: " << (int)(value * 360) << std::endl;
	});
	windDirectionTextBox->setFixedSize(nanogui::Vector2i(70, 25));
	windDirectionTextBox->setFontSize(20);
	windDirectionTextBox->setAlignment(nanogui::TextBox::Alignment::Right);

	// Hook slider
	new nanogui::Label(window, "Wind strength", "sans-bold");

	nanogui::Widget *windStrengthPanel = new nanogui::Widget(window);
	windStrengthPanel->setLayout(new nanogui::BoxLayout(
		nanogui::Orientation::Horizontal,
		nanogui::Alignment::Middle, 0, 20));

	// Wind strength slider
	nanogui::Slider *windStrengthSlider = new nanogui::Slider(windStrengthPanel);
	windStrengthSlider->setValue(0.0f);
	windStrengthSlider->setFixedWidth(180);

	nanogui::TextBox *windStrengthTextBox = new nanogui::TextBox(windStrengthPanel);
	windStrengthTextBox->setValue("0");
	windStrengthTextBox->setUnits("");
	// On slider move
	windStrengthSlider->setCallback([&, windStrengthTextBox](float value) {
		windStrengthTextBox->setValue(std::to_string(value));
		m_clothSystem->windForce = value;
		
	});
	// On slider release
	windStrengthSlider->setFinalCallback([&](float value) {
		std::cout << "Final slider value: " << value << std::endl;
	});
	windStrengthTextBox->setFixedSize(nanogui::Vector2i(80, 25));
	windStrengthTextBox->setFontSize(20);
	windStrengthTextBox->setAlignment(nanogui::TextBox::Alignment::Right);

	// Fold slider
	new nanogui::Label(window, "Corner distance", "sans-bold");

	nanogui::Widget *foldPanel = new nanogui::Widget(window);
	foldPanel->setLayout(new nanogui::BoxLayout(
		nanogui::Orientation::Horizontal,
		nanogui::Alignment::Middle, 0, 20));

	// Wind strength slider
	m_foldSlider = new nanogui::Slider(foldPanel);
	m_foldSlider->setValue(1.0f);
	m_foldSlider->setFixedWidth(180);

	m_foldTextBox = new nanogui::TextBox(foldPanel);
	m_foldTextBox->setValue("2");
	m_foldTextBox->setUnits("");
	// On slider move
	m_foldSlider->setCallback([&](float value) {
		m_foldTextBox->setValue(std::to_string((value + 0.001) * 2));
		for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
			Entity& clothEntity = m_scene.getEntity(i);
			if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
				for (size_t i = 0; i < m_clothPointsX; i++)
				{
					PointMass& pointMass = clothEntity.cloth.getNode(i).pointMass;
					if (pointMass.isFixed) {
						glm::vec3 pointPos = pointMass.getPosition();
						glm::vec3 leftCornerPos = clothEntity.cloth.getNode(0).pointMass.getPosition();
						glm::vec3 rightCornerPos = leftCornerPos + glm::vec3{m_clothSize, 0.0f, 0.0f};
						float alpha = static_cast<float>(i) / (m_clothPointsX - 1);
						float interpolatedX = lerp(leftCornerPos.x, rightCornerPos.x, alpha * (value + 0.001));
						pointMass.setPosition({ interpolatedX, pointPos.y, pointPos.z});
					}
				}
			}
		}
	});
	// On slider release
	m_foldSlider->setFinalCallback([&](float value) {
		std::cout << "Final slider value: " << (value + 0.001) * 2 << std::endl;
	});
	m_foldTextBox->setFixedSize(nanogui::Vector2i(80, 25));
	m_foldTextBox->setFontSize(20);
	m_foldTextBox->setAlignment(nanogui::TextBox::Alignment::Right);

	nanogui::Button *releaseButton = new nanogui::Button(window, "Release");
	// On button press
	releaseButton->setCallback([&] {
		std::cout << "pushed!" << std::endl;
		for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
			Entity& clothEntity = m_scene.getEntity(i);
			if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
				for (size_t i = 0; i < m_clothPointsX; i++)
				{
					PointMass& pointMass = clothEntity.cloth.getNode(i).pointMass;
					pointMass.isFixed = false;
				}
			}
		}
	});
	releaseButton->setTooltip("releases the hooks");

	m_uiScreen->setVisible(true);
	m_uiScreen->performLayout();
}

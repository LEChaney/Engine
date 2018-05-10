//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of OpenGL utilty functions.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "Texture.h"
#include "Shader.h"

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>
#include <string>

struct VertexFormat;
struct GLFWwindow;
class Scene;
class InputSystem;

namespace GLUtils {
	// Initializes the window, opengl context and opengl function pointers
	GLFWwindow* initOpenGL();

	// Returns a handler to the default shader.
	// This function will build the shader if it is not already built.
	const Shader& getDefaultShader();

	// Returns a handler to a shader that can take a metallicness map.
	// This function will build the shader if it is not already built.
	const Shader& getMetalShader();

	// Returns a handler to the debug shader.
	// This shader can draw meshes with a solid color.
	// Change the materials debug color property to set which color to use.
	// This function will build the shader if it is not already built.
	const Shader& getDebugShader();

	// Returns a handler to the skybox shader.
	// This function will build the sahder if it is not already built.
	const Shader& getSkyboxShader();

	// Retuns a shader designed to render a full screen quad using 
	// the currently bound texture.
	const Shader& getFullscreenQuadShader();

	// Retuns an edge detection post processing shader.
	// This should be applied to a full screen quad with scene color buffer bound
	// in texture unit 0.
	const Shader& getPPEdgeDetectShader();

	// Retures a shader used to generate grass from a terrain mesh.
	const Shader& getTerrainGrassGeoShader();

	// Returns a shader used to render tessellated heightmapped terrain.
	const Shader& getTerrainShader();

	// Helper function for creating a tesselated quad
	void createTessellatedQuadData(GLsizei numVertsX, GLsizei numVertsZ, float width, float height, std::vector<VertexFormat>& vertices, std::vector<GLuint>& indices);

	// Buffers vertex and index data to the GPU.
	// Returns a handler the the VAO associated with the vertices / indices.
	GLuint bufferMeshData(const std::vector<VertexFormat>& vertices, const std::vector<GLuint>& indices);

	// Loads a texture to GPU memory.
	// Returns a texture object with GPU handler and texture target.
	Texture loadTexture(const std::string& paths, bool sRGB = true, bool generateMipmaps = true);

	// Loads a cube map to GPU memory.
	// Returns a texture object with GPU handler and texture target.
	Texture loadCubeMapFaces(const std::vector<std::string>& facePaths);

	// Loads a texture in DDS format to to GPU memory.
	// Filename can be KTX or DDS files.
	// Returns a texture object with GPU handler and texture target.
	Texture loadDDSTexture(const std::string& path);
}
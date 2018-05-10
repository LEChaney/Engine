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

#include "GLUtils.h"

#include "Material.h"
#include "Mesh.h"
#include "ShaderHelper.h"
#include "VertexFormat.h"
#include "Shader.h"
#include "stb_image.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <gli\gli.hpp>

#include "Log.h"
#include <unordered_map>
#include <string>
#include <sstream>

#define BUFFER_OFFSET(i) ((GLvoid *)(i*sizeof(float)))

// Callback for handling glfw errors
void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

// Handles glContext resize events
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLFWwindow* GLUtils::initOpenGL()
{
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Create opengl glContext and glContext
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* glContext = glfwCreateWindow(1400, 800, "Doge-otron 2017", nullptr, nullptr);
	if (!glContext)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(glContext);

	// Register callbacks
	//glfwSetKeyCallback(glContext, keyCallback);
	glfwSetFramebufferSizeCallback(glContext, framebufferSizeCallback);

	// Load opengl functinos
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Configure glContext
	glfwSwapInterval(1);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Setup opengl viewport
	int width, height;
	glfwGetFramebufferSize(glContext, &width, &height);
	glViewport(0, 0, width, height);

	return glContext;
}

const Shader& GLUtils::getDefaultShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/default_vert.glsl",
		"Assets/Shaders/default_frag.glsl");

	return s_shader;
}

const Shader& GLUtils::getMetalShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/default_vert.glsl",
		"Assets/Shaders/metal_frag.glsl");

	return s_shader;
}

const Shader& GLUtils::getDebugShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/default_vert.glsl",
		"Assets/Shaders/debug_frag.glsl");

	return s_shader;
}

const Shader& GLUtils::getSkyboxShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/skybox_vert.glsl",
		"Assets/Shaders/skybox_frag.glsl");

	return s_shader;
}

const Shader& GLUtils::getFullscreenQuadShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/fullscreen_quad_vert.glsl",
		"Assets/Shaders/fullscreen_quad_frag.glsl");

	return s_shader;
}

const Shader& GLUtils::getPPEdgeDetectShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/fullscreen_quad_vert.glsl",
		"Assets/Shaders/pp_edge_detect_frag.glsl");

	return s_shader;
}

const Shader& GLUtils::getTerrainGrassGeoShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/terrain_vert.glsl",
		"Assets/Shaders/grass_frag.glsl",
		"Assets/Shaders/terrain_tess_ctrl.glsl",
		"Assets/SHaders/terrain_tess_eval.glsl",
		"Assets/Shaders/grass_geo.glsl");

	return s_shader;
}

const Shader& GLUtils::getTerrainShader()
{
	static Shader s_shader = compileAndLinkShaders(
		"Assets/Shaders/terrain_vert.glsl",
		"Assets/Shaders/default_frag.glsl",
		"Assets/Shaders/terrain_tess_ctrl.glsl",
		"Assets/SHaders/terrain_tess_eval.glsl");

	return s_shader;
}

void GLUtils::createTessellatedQuadData(GLsizei numVertsX, GLsizei numVertsZ, float width, float height, std::vector<VertexFormat>& vertices, std::vector<GLuint>& indices)
{
	{
		const GLsizei numCellsX = numVertsX - 1;
		const GLsizei numCellsZ = numVertsZ - 1;
		const GLsizei numIndicesPerCell = 6;
		const GLsizei numIndices = numCellsX * numCellsZ * numIndicesPerCell;
		const float xExtent = width / 2;
		const float zExtent = height / 2;
		const float xSpacing = width / (numVertsX - 1);
		const float zSpacing = height / (numVertsZ - 1);
		vertices.resize(numVertsX * numVertsZ);

		// Create vertices
		for (GLsizei r = 0; r < numVertsZ; ++r) {
			for (GLsizei c = 0; c < numVertsX; ++c) {
				float x = -xExtent + c * xSpacing;
				float z = -zExtent + r * zSpacing;
				vertices[r * numVertsX + c].position = glm::vec3{ x, 0, z };
				vertices[r * numVertsX + c].normal = glm::vec3{ 0, 1, 0 };
				vertices[r * numVertsX + c].texCoord = glm::vec2{ (x + xExtent) / width, (z + zExtent) / height };
			}
		}

		// Create indices
		indices.resize(numIndices);
		for (GLsizei r = 0; r < numCellsZ; ++r) {
			for (GLsizei c = 0; c < numCellsX; ++c) {
				indices[(r * numCellsX + c) * numIndicesPerCell] = r * numVertsX + c;
				indices[(r * numCellsX + c) * numIndicesPerCell + 1] = (r + 1) * numVertsX + c;
				indices[(r * numCellsX + c) * numIndicesPerCell + 2] = (r + 1) * numVertsX + c + 1;

				indices[(r * numCellsX + c) * numIndicesPerCell + 3] = r * numVertsX + c;
				indices[(r * numCellsX + c) * numIndicesPerCell + 4] = (r + 1) * numVertsX + c + 1;
				indices[(r * numCellsX + c) * numIndicesPerCell + 5] = r * numVertsX + c + 1;
			}
		}
	}
}

GLuint GLUtils::bufferMeshData(const std::vector<VertexFormat>& vertices, const std::vector<GLuint>& indices)
{
	GLuint VAO;
	GLuint buffers[2];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &buffers[0]);
	glGenBuffers(1, &buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	
	GLuint positionLoc = 0;
	GLuint normalLoc = 1;
	GLuint texCoordLoc = 2;
	GLuint colorLoc = 3;

	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(0));
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(3));
	glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(6));
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(8));

	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(normalLoc);
	glEnableVertexAttribArray(texCoordLoc);
	glEnableVertexAttribArray(colorLoc);

	return VAO;
}

Texture GLUtils::loadTexture(const std::string& path, bool sRGB, bool generateMipmaps)
{
	// Cached textures that have already been loaded
	static std::unordered_map<std::string, Texture> s_loadedTextures;

	// A texture with the same filepath has already been loaded, return a copy. (optimization)
	if (s_loadedTextures.find(path) != s_loadedTextures.end())
		return s_loadedTextures.at(path);

	Texture texture;
	texture.target = GL_TEXTURE_2D;

	//stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char* textureData = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

	if (textureData) {
		GLenum format;
		GLint internalFormat;
		if (nrComponents == 1) {
			format = GL_RED;
			internalFormat = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
			if (sRGB)
				internalFormat = GL_SRGB;
			else
				internalFormat = format;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
			if (sRGB)
				internalFormat = GL_SRGB_ALPHA;
			else
				internalFormat = format;
		}

		glGenTextures(1, &texture.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(texture.target, texture.id);

		if (generateMipmaps) {
			glTexParameteri(texture.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(texture.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glTexImage2D(texture.target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);

		if (generateMipmaps)
			glGenerateMipmap(texture.target);

		glBindTexture(texture.target, 0);

		s_loadedTextures.insert(std::make_pair(path, texture));
	} else {
		// TODO: Throw excpetion here
		g_log << "Texture failed to load at path: " << path << "\n";
	}

	stbi_image_free(textureData);

	return texture;
}

// TODO: Combine with load textures for easier changability
Texture GLUtils::loadCubeMapFaces(const std::vector<std::string>& facePaths)
{
	// Cached textures that have already been loaded
	static std::unordered_map<std::string, Texture> s_loadedTextures;

	// Combine filepaths to create id
	std::ostringstream oss;
	for (const std::string& path : facePaths)
		oss << path;
	std::string id = oss.str();

	// A texture with the same id has already been loaded, return a copy. (optimization)
	if (s_loadedTextures.find(id) != s_loadedTextures.end())
		return s_loadedTextures.at(id);

	Texture texture;
	texture.target = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &texture.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture.target, texture.id);

	glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (GLenum i = 0; i < facePaths.size(); ++i) {
		//stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		unsigned char* faceData = stbi_load(facePaths.at(i).c_str(), &width, &height, 
		                                    &nrComponents, 0);

		if (faceData) {
			GLenum format;
			GLint internalFormat;
			if (nrComponents == 1) {
				format = GL_RED;
				internalFormat = GL_RED;
			}
			else if (nrComponents == 3) {
				format = GL_RGB;
				internalFormat = GL_SRGB;
			}
			else if (nrComponents == 4) {
				format = GL_RGBA;
				internalFormat = GL_SRGB_ALPHA;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, faceData);
		} else {
			// TODO: Throw excpetion here
			g_log << "Texture failed to load at path: " << facePaths.at(i) << "\n";
		}

		stbi_image_free(faceData);
	}

	glBindTexture(texture.target, 0);

	s_loadedTextures.insert(std::make_pair(id, texture));
	return texture;
}

Texture GLUtils::loadDDSTexture(const std::string& path)
{
	Texture finalTexture{};

	gli::texture texture = gli::load(path);
	if (texture.empty()) {
		// TODO: Throw error here
		g_log << "Texture failed to load at path: " << path << "\n";
		return finalTexture;
	}

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format format = GL.translate(texture.format(), texture.swizzles());
	GLenum target = GL.translate(texture.target());

	// Slightly hacky fix for sRGB encoded cubemaps
	if (format.Internal == gli::gl::INTERNAL_RGBA8_UNORM) {
		format.Internal = gli::gl::INTERNAL_SRGB8_ALPHA8;
	}
	if (format.Internal == gli::gl::INTERNAL_RGB8_UNORM) {
		format.Internal = gli::gl::INTERNAL_SRGB8;
	}

	GLuint textureName = 0;
	glGenTextures(1, &textureName);
	glBindTexture(target, textureName);
	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
	glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
	glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
	glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
	glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	finalTexture.id = textureName;
	finalTexture.target = target;

	glm::tvec3<GLsizei> const extent(texture.extent());
	GLsizei const faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());

	switch (texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(
			target, static_cast<GLint>(texture.levels()), format.Internal, extent.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(
			target, static_cast<GLint>(texture.levels()), format.Internal, extent.x, 
			texture.target() != gli::TARGET_1D_ARRAY ? extent.y : faceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(
			target, static_cast<GLint>(texture.levels()), format.Internal,
			extent.x, extent.y,
			texture.target() == gli::TARGET_3D ? extent.z : faceTotal);
		break;
	default:
		assert(0);
		break;
	}

	for (std::size_t layer = 0; layer < texture.layers(); ++layer) {
		for (std::size_t face = 0; face < texture.faces(); ++face) {
			for (std::size_t level = 0; level < texture.levels(); ++level)
			{
				GLsizei const layerGL = static_cast<GLsizei>(layer);
				glm::tvec3<GLsizei> extent(texture.extent(level));
				target = gli::is_target_cube(texture.target())
					? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
					: target;

				switch (texture.target())
				{
				case gli::TARGET_1D:
					if (gli::is_compressed(texture.format()))
						glCompressedTexSubImage1D(
							target, static_cast<GLint>(level), 0, extent.x,
							format.Internal, static_cast<GLsizei>(texture.size(level)),
							texture.data(layer, face, level));
					else
						glTexSubImage1D(
							target, static_cast<GLint>(level), 0, extent.x,
							format.External, format.Type,
							texture.data(layer, face, level));
					break;
				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
					if (gli::is_compressed(texture.format()))
						glCompressedTexSubImage2D(
							target, static_cast<GLint>(level),
							0, 0,
							extent.x,
							texture.target() == gli::TARGET_1D_ARRAY ? layerGL : extent.y,
							format.Internal, static_cast<GLsizei>(texture.size(level)),
							texture.data(layer, face, level));
					else
						glTexSubImage2D(
							target, static_cast<GLint>(level),
							0, 0,
							extent.x,
							texture.target() == gli::TARGET_1D_ARRAY ? layerGL : extent.y,
							format.External, format.Type,
							texture.data(layer, face, level));
					break;
				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
					if (gli::is_compressed(texture.format()))
						glCompressedTexSubImage3D(
							target, static_cast<GLint>(level),
							0, 0, 0,
							extent.x, extent.y,
							texture.target() == gli::TARGET_3D ? extent.z : layerGL,
							format.Internal, static_cast<GLsizei>(texture.size(level)),
							texture.data(layer, face, level));
					else
						glTexSubImage3D(
							target, static_cast<GLint>(level),
							0, 0, 0,
							extent.x, extent.y,
							texture.target() == gli::TARGET_3D ? extent.z : layerGL,
							format.External, format.Type,
							texture.data(layer, face, level));
					break;
				default: assert(0); break;
				}
			}
		}
	}

	return finalTexture;
}

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glad\glad.h>

#include <unordered_map>
#include <string>

class Shader
{
public:
	Shader(GLuint gpuProgramHandle, GLuint gpuShadowProgramHandle, bool hasTessellationStage = false);
	~Shader();

	// Make this shader the current shader for rendering
	void use() const;

	// Make this shader's shadow version the current shader for rendering
	// (Same shader, but excludes fragment stage)
	void useShadow() const;
	
	// Returns a handle to the GPU shader object
	GLuint getGPUHandle() const;

	// Returns a handle to the GPU shadow shader object
	GLuint getGPUShadowShaderHandle() const;

	// Returns a handle to the specified uniform object on the GPU
	GLint getUniformLocation(const std::string& uniformName) const;

	// Returns a handle to the specified uniform block on the GPU
	GLuint getUniformBlockIndex(const std::string& uniformBlockName) const;

	// Returns true is this shader includes a tesselation stageW
	bool hasTessellationStage() const;

private:
	GLuint m_gpuHandle;
	GLuint m_gpuShadowShaderHandle;
	bool m_hasTessellationStage;
	mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
	mutable std::unordered_map<std::string, GLuint> m_uniformBlockIndexCache;
};


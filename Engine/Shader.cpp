#include "Shader.h"

#include <glad\glad.h>

#include <string>

Shader::Shader(GLuint gpuProgramHandle, GLuint gpuShadowProgramHandle, bool hasTessellationStage)
	: m_gpuHandle{ gpuProgramHandle }
	, m_gpuShadowShaderHandle{ gpuShadowProgramHandle }
	, m_hasTessellationStage{ hasTessellationStage }
{
}

Shader::~Shader()
{
}

void Shader::use() const
{
	return glUseProgram(m_gpuHandle);
}

void Shader::useShadow() const
{
	return glUseProgram(m_gpuShadowShaderHandle);
}

GLuint Shader::getGPUHandle() const
{
	return m_gpuHandle;
}

GLuint Shader::getGPUShadowShaderHandle() const
{
	return m_gpuShadowShaderHandle;
}

GLint Shader::getUniformLocation(const std::string& uniformName) const
{
	auto searchResult = m_uniformLocationCache.find(uniformName);
	if (searchResult != m_uniformLocationCache.end())
		return searchResult->second;

	auto location = glGetUniformLocation(m_gpuHandle, uniformName.c_str());
	m_uniformLocationCache[uniformName] = location;
	return location;
}

GLuint Shader::getUniformBlockIndex(const std::string& uniformBlockName) const
{
	auto searchResult = m_uniformBlockIndexCache.find(uniformBlockName);
	if (searchResult != m_uniformBlockIndexCache.end())
		return searchResult->second;


	auto location = glGetUniformBlockIndex(m_gpuHandle, uniformBlockName.c_str());
	m_uniformBlockIndexCache[uniformBlockName] = location;
	return location;
}

bool Shader::hasTessellationStage() const
{
	return m_hasTessellationStage;
}

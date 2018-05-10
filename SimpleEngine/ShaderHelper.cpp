//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Utils for shader operations
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "ShaderHelper.h"

#include "Log.h"
#include "Shader.h"

#include <fstream>
#include <assert.h>
#include <vector>

std::string readShaderFileFromResource(const std::string& fileName) {
	std::string outFile;
	try {
		std::ifstream f(fileName);
		std::string line;
		while (std::getline(f, line)) {
			outFile.append(line);
			outFile.append("\n");
		}
		f.close();
	}
	catch (std::ifstream::failure e) {
		g_log << "Exception opening/reading/closing file\n";
	}
	return outFile;
}

GLuint compileShader(GLenum ShaderType, const std::string& shaderCode) {
	const  GLuint shaderObjectId = glCreateShader(ShaderType);
	if (shaderObjectId == 0) {
		g_log << "Error creating shader type " << ShaderType << "\n";
		assert(false);
		exit(0);
	}
	const GLchar* p[1];
	p[0] = shaderCode.c_str();
	GLint Lengths[1];
	Lengths[0] = static_cast<GLint>(strlen(shaderCode.c_str()));

	glShaderSource(shaderObjectId, 1, p, Lengths);
	glCompileShader(shaderObjectId);
	GLint compileStatus;
	glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compileStatus);
	if (!compileStatus) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shaderObjectId, 1024, NULL, InfoLog);
		g_log << "Error compiling shader type" << ShaderType << "\n" << InfoLog << "\n";
		assert(false);
		exit(1);
	}

	return shaderObjectId;
}

GLuint linkProgram(const std::vector<GLuint>& shadersToLink) {
	GLint linkStatus = 0;
	GLchar ErrorLog[1024] = { 0 };
	const GLuint programObjectId = glCreateProgram();

	if (programObjectId == 0) {
		g_log << "Error creating shader program \n";
		assert(false);
		exit(1);
	}

	for (GLuint shaderId : shadersToLink)
		glAttachShader(programObjectId, shaderId);
	glLinkProgram(programObjectId);

	glGetProgramiv(programObjectId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == 0) {
		glGetProgramInfoLog(programObjectId, sizeof(ErrorLog), NULL, ErrorLog);
		g_log << "Error linking shader program: \n" << ErrorLog << "\n";
		assert(false);
		exit(1);
	}
	return programObjectId;
}

GLint validateProgram(GLuint programObjectId) {
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glValidateProgram(programObjectId);
	glGetProgramiv(programObjectId, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(programObjectId, sizeof(ErrorLog), NULL, ErrorLog);
		g_log << "Invalid shader program: \n" << ErrorLog;
		assert(false);
		exit(1);
	}
	return Success;
}

Shader compileAndLinkShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile,
                             const char* tessCtrlShaderFile, const char* tessEvalShaderFile, const char* geometryShaderFile) {
	std::string vertexShaderSource = readShaderFileFromResource(vertexShaderFile);
	std::string fragmentShaderSource = readShaderFileFromResource(fragmentShaderFile);

	std::vector<GLuint> shadersToLink;
	shadersToLink.push_back(compileShader(GL_VERTEX_SHADER, vertexShaderSource));
	shadersToLink.push_back(compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource));

	if (tessCtrlShaderFile) {
		std::string tessellationShaderSource = readShaderFileFromResource(tessCtrlShaderFile);
		shadersToLink.push_back(compileShader(GL_TESS_CONTROL_SHADER, tessellationShaderSource));
	}

	if (tessEvalShaderFile) {
		std::string tessellationShaderSource = readShaderFileFromResource(tessEvalShaderFile);
		shadersToLink.push_back(compileShader(GL_TESS_EVALUATION_SHADER, tessellationShaderSource));
	}

	if (geometryShaderFile) {
		std::string geometryShaderSource = readShaderFileFromResource(geometryShaderFile);
		shadersToLink.push_back(compileShader(GL_GEOMETRY_SHADER, geometryShaderSource));
	}
	
	GLuint program = linkProgram(shadersToLink);
	
	for (GLuint shaderId : shadersToLink)
		glDeleteShader(shaderId);

	if (tessCtrlShaderFile || tessEvalShaderFile)
		return Shader(program, true);
	else
		return Shader(program, false);
}
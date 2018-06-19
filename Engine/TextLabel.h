#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderHelper.h"

#include"Utils.h"


#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <map>
#include <string>
#include "Log.h"

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};



class TextLabel
{
public:
	TextLabel(std::string text, std::string font);
	~TextLabel();

	void Render();
	void setPosition(glm::vec2 _position);
	void setColor(glm::vec3 _color);
	void setScale(GLfloat _scale);
	void setText(std::string _text);

private:
	std::string m_text;
	GLfloat m_scale;
	glm::vec3 m_color;
	glm::vec2 m_position;

	GLuint m_VAO, m_VBO;
	Shader m_shader;
	std::map<GLchar, Character> m_characters;
};


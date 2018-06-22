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

#include "Material.h"

#include "GLUtils.h"

Material::Material()
	: shader{ &GLUtils::getDebugShader() }
	, shaderParams{}
	, willDrawDepth{ true }
	, debugColor{ 0, 1, 0 }
	, heightMapScale{ 100 }
	, willDrawWireframe{ false }
{
}

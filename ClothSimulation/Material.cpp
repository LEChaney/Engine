#include "Material.h"

#include "GLUtils.h"

Material::Material()
	: shader{ &GLUtils::getDebugShader() }
	, shaderParams{}
	, willDrawDepth{ true }
	, debugColor{ 1, 0, 0 }
	, heightMapScale{ 100 }
	, willDrawWireframe{ false }
{
}

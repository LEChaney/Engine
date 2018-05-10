#pragma once

#include <string>

struct ModelComponent;

namespace ModelUtils {
	ModelComponent loadModel(const std::string& path);
}
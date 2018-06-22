#pragma once

#include "ssAnimatedModel.h"

#include <memory>

struct AnimatedModelComponent {
	AnimatedModelComponent();

	std::unique_ptr<ssAnimatedModel> model;
};
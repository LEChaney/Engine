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

#include "ssAnimatedModel.h"

#include <memory>

struct AnimatedModelComponent {
	AnimatedModelComponent();

	std::unique_ptr<ssAnimatedModel> model;
};
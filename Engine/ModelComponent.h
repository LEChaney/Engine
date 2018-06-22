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

#include "Mesh.h"
#include "Material.h"

#include <vector>

struct ModelComponent {
	// The root node of the models scene tree.
	// Not currently used.
	MeshNode rootNode;
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};
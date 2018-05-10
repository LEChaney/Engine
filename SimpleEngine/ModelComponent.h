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
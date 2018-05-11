//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A component for storing per entity
//                mesh information used by the GPU.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glad\glad.h>

#include <vector>

struct Mesh {
	unsigned int materialIndex;
	GLuint VAO;
	GLsizei numIndices;
};

// A tree structure of mesh nodes.
// Mesh nodes only contain mesh ids and child nodes.
// The actual meshes can be access by indexing into the
// corosponding model components mesh array using the meshID.
struct MeshNode {
	std::vector<unsigned int> meshIDs;
	std::vector<MeshNode> childNodes;
};
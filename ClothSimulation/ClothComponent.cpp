#include "ClothComponent.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"

ClothLink::ClothLink(GLuint node1Id, GLuint node2Id, GLuint springConstraintId)
	: node1Id{ node1Id }
	, node2Id{ node2Id }
	, springConstraintId{ springConstraintId }
{
}

ClothNode::ClothNode(glm::vec3 position, GLfloat mass, GLboolean isFixed)
	: pointMass(position, mass, isFixed)
{
}

void ClothComponent::addClothLink(GLuint node1Id, GLuint node2Id, GLfloat stiffness, GLfloat breakDistance)
{
	// Add the new constraint to the constraint list
	m_springConstraints.emplace_back(m_clothNodes[node1Id].pointMass, m_clothNodes[node2Id].pointMass, stiffness, breakDistance);
	
	// Construct clothlink wrapper
	GLuint constraintId = m_springConstraints.size() - 1;
	ClothLink clothLink(node1Id, node2Id, constraintId);
	
	// Calculate cardinal direction of link
	glm::vec3 pos1 = m_clothNodes[node1Id].pointMass.getPosition();
	glm::vec3 pos2 = m_clothNodes[node2Id].pointMass.getPosition();
	glm::vec3 offsetFromP1ToP2 = pos2 - pos1;
	if (offsetFromP1ToP2.x == 0 && offsetFromP1ToP2.z < 0) {
		// North link from 1 to 2
		m_clothNodes[node1Id].linkDirections[0].push_back(clothLink);
		// South link from 2 to 1
		m_clothNodes[node2Id].linkDirections[4].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x > 0 && offsetFromP1ToP2.z < 0) {
		// North East link from 1 to 2
		m_clothNodes[node1Id].linkDirections[1].push_back(clothLink);
		// South West link from 2 to 1
		m_clothNodes[node2Id].linkDirections[5].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x > 0 && offsetFromP1ToP2.z == 0) {
		// East link from 1 to 2
		m_clothNodes[node1Id].linkDirections[2].push_back(clothLink);
		// West link from 2 to 1
		m_clothNodes[node2Id].linkDirections[6].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x > 0 && offsetFromP1ToP2.z > 0) {
		// South East link from 1 to 2
		m_clothNodes[node1Id].linkDirections[3].push_back(clothLink);
		// North West link from 2 to 1
		m_clothNodes[node2Id].linkDirections[7].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x == 0 && offsetFromP1ToP2.z > 0) {
		// South link from 1 to 2
		m_clothNodes[node1Id].linkDirections[4].push_back(clothLink);
		// North link from 2 to 1
		m_clothNodes[node2Id].linkDirections[0].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x < 0 && offsetFromP1ToP2.z > 0) {
		// South West link from 1 to 2
		m_clothNodes[node1Id].linkDirections[5].push_back(clothLink);
		// North East link from 2 to 1
		m_clothNodes[node2Id].linkDirections[1].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x < 0 && offsetFromP1ToP2.z == 0) {
		// West link from 1 to 2
		m_clothNodes[node1Id].linkDirections[6].push_back(clothLink);
		// East link from 2 to 1
		m_clothNodes[node2Id].linkDirections[2].push_back(clothLink);
	}
	else if (offsetFromP1ToP2.x < 0 && offsetFromP1ToP2.z < 0) {
		// North West link from 1 to 2
		m_clothNodes[node1Id].linkDirections[7].push_back(clothLink);
		// South East link from 2 to 1
		m_clothNodes[node2Id].linkDirections[3].push_back(clothLink);
	}
}

void ClothComponent::breakClothLink(const ClothLink& linkToBreak)
{
	// Remove actual spring constraint
	m_springConstraints.erase(m_springConstraints.begin() + linkToBreak.springConstraintId);

	// Remove link from cloth graph in one direction
	bool wasBreak = false;
	for (auto& clothLinks : m_clothNodes.at(linkToBreak.node1Id).linkDirections) {
		for (size_t i = 0; i < clothLinks.size(); ++i) {
			if (clothLinks[i].node2Id == linkToBreak.node2Id) {
				clothLinks.erase(clothLinks.begin() + i);
				
				wasBreak = true;
				break;
			}
		}
		if (wasBreak)
			break;
	}

	// Remove link from cloth graph going in opposite direction
	for (auto& clothLinks : m_clothNodes.at(linkToBreak.node2Id).linkDirections) {
		for (size_t i = 0; i < clothLinks.size(); ++i) {
			if (clothLinks[i].node1Id == linkToBreak.node1Id) {
				clothLinks.erase(clothLinks.begin() + i);
				return;
			}
		}
	}
}

void ClothComponent::breakAllLinksInDirection(GLuint nodeId, LinkDirection direction)
{
	size_t dirIdx = static_cast<size_t>(direction);

	// Remove actual spring constraints
	for (auto& clothLink : m_clothNodes[nodeId].linkDirections[dirIdx]) {
		m_springConstraints.erase(m_springConstraints.begin() + clothLink.springConstraintId);
	}

	// Clear links from cloth graph
	m_clothNodes[nodeId].linkDirections[dirIdx].clear();
}

bool ClothComponent::hasConstraintBetween(GLuint idx1, GLuint idx2) const
{
	for (auto& clothLinks : m_clothNodes[idx1].linkDirections) {
		for (auto& clothLink : clothLinks) {
			if (clothLink.node2Id == idx2)
				return true;
		}
	}

	return false;
}

Entity& ClothComponent::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea)
{
	Entity& clothEntity = scene.createEntity(COMPONENT_CLOTH, COMPONENT_MODEL);
	ClothComponent& cloth = clothEntity.cloth;
	cloth.m_numPointMassesX = numPointsX;
	cloth.m_numPointMassesY = numPointsY;

	// Calculate each point mass's individual mass
	GLuint numPoints = numPointsX * numPointsY;
	GLfloat separationX = width / (numPointsX - 1);
	GLfloat separationY = height / (numPointsY - 1);
	GLfloat patchMass = separationX * separationY * weightPerUnitArea; // Weight for a 2 x 2 patch of point masses
	GLfloat individualMass = patchMass / 4.0f; // Assuming square patches

	// Create a tessellated quad to build the cloth out of
	std::vector<VertexFormat> vertices;
	GLUtils::createTessellatedQuadData(numPointsX, numPointsY, width, height, vertices, cloth.m_triIndices);

	// Buffer triangle mesh to GPU for rendering
	clothEntity.model.rootNode.meshIDs.push_back(0);
	clothEntity.model.meshes.push_back(GLUtils::bufferMeshData(vertices, cloth.m_triIndices, GL_STREAM_DRAW));

	// Set Material to use for drawing
	Material material;
	material.willDrawWireframe = true;
	clothEntity.model.materials.push_back(material);

	// Create point masses from cloth vertices 
	cloth.m_clothNodes.reserve(vertices.size());
	for (GLuint i = 0; i < vertices.size(); ++i) {
		cloth.m_clothNodes.emplace_back(vertices[i].position, individualMass, false);

		// Fix first 4 verts
		if (i < numPointsX)
			cloth.m_clothNodes[i].pointMass.isFixed = true;
	}

	// Connect point masses with spring constraints
	for (GLuint i = 0; i < cloth.m_clothNodes.size(); ++i) {
		GLuint topLeftIdx = i;
		GLuint topRightIdx = i + 1;
		GLuint bottomLeftIdx = i + cloth.m_numPointMassesX;
		GLuint bottomRightIdx = i + cloth.m_numPointMassesX + 1;

		// Structural Constraints
		cloth.addClothLink(topLeftIdx, topRightIdx, 1.0f, 0.25f);
		cloth.addClothLink(topLeftIdx, bottomLeftIdx, 1.0f, 0.25f);

		// Shear Constraints
		//cloth.addSpringConstraint(r, c, r + 1, c + 1, 0.5f, 0.25f);
		//cloth.addSpringConstraint(r, c + 1, r + 1, c, 0.5f, 0.25f);

		// Bending Constraints
		//if (c < numPointsX - 2)
		//	cloth.addSpringConstraint(r, c, r, c + 2, 0.1f, 0.25f);
		//if (r < numPointsY - 2)
		//	cloth.addSpringConstraint(r, c, r + 2, c, 0.1f, 0.25f);
		//if (r < numPointsY - 2 && c < numPointsX - 2) {
		//	cloth.addSpringConstraint(r, c, r + 2, c + 2, 0.1f, 0.25f);
		//	cloth.addSpringConstraint(r, c + 2, r + 2, c, 0.1f, 0.25f);
		//}
	}

	return clothEntity;
}

Entity& Prefabs::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight)
{
	return ClothComponent::createCloth(scene, numPointsX, numPointsY, width, height, totalWeight);
}

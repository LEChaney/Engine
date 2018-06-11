#include "ClothComponent.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"

ClothLink::ClothLink(GLuint node1Id, GLuint node2Id, GLint linkDirection, const SpringConstraint& springConstraint)
	: node1Id{ node1Id }
	, node2Id{ node2Id }
	, direction{ linkDirection }
	, springConstraint{ springConstraint }
{
}

ClothNode::ClothNode(glm::vec3 position, GLfloat mass, GLboolean isFixed)
	: pointMass(position, mass, isFixed)
{
}

void ClothComponent::addClothLink(GLuint node1Id, GLuint node2Id, GLfloat stiffness, GLfloat breakDistance)
{
	GLuint clothLinkId = clothLinks.size();
	
	// Calculate cardinal direction of link
	GLint linkDir;
	glm::vec3 pos1 = clothNodes[node1Id].pointMass.getPosition();
	glm::vec3 pos2 = clothNodes[node2Id].pointMass.getPosition();
	glm::vec3 offsetFromP1ToP2 = pos2 - pos1;
	if (offsetFromP1ToP2.x == 0 && offsetFromP1ToP2.z < 0) {
		// North link from 1 to 2
		linkDir = 0;
		clothNodes[node1Id].linkDirections[0].push_back(clothLinkId);
		// South link from 2 to 1
		clothNodes[node2Id].linkDirections[4].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x > 0 && offsetFromP1ToP2.z < 0) {
		// North East link from 1 to 2
		linkDir = 1;
		clothNodes[node1Id].linkDirections[1].push_back(clothLinkId);
		// South West link from 2 to 1
		clothNodes[node2Id].linkDirections[5].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x > 0 && offsetFromP1ToP2.z == 0) {
		// East link from 1 to 2
		linkDir = 2;
		clothNodes[node1Id].linkDirections[2].push_back(clothLinkId);
		// West link from 2 to 1
		clothNodes[node2Id].linkDirections[6].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x > 0 && offsetFromP1ToP2.z > 0) {
		// South East link from 1 to 2
		linkDir = 3;
		clothNodes[node1Id].linkDirections[3].push_back(clothLinkId);
		// North West link from 2 to 1
		clothNodes[node2Id].linkDirections[7].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x == 0 && offsetFromP1ToP2.z > 0) {
		// South link from 1 to 2
		linkDir = 4;
		clothNodes[node1Id].linkDirections[4].push_back(clothLinkId);
		// North link from 2 to 1
		clothNodes[node2Id].linkDirections[0].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x < 0 && offsetFromP1ToP2.z > 0) {
		// South West link from 1 to 2
		linkDir = 5;
		clothNodes[node1Id].linkDirections[5].push_back(clothLinkId);
		// North East link from 2 to 1
		clothNodes[node2Id].linkDirections[1].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x < 0 && offsetFromP1ToP2.z == 0) {
		// West link from 1 to 2
		linkDir = 6;
		clothNodes[node1Id].linkDirections[6].push_back(clothLinkId);
		// East link from 2 to 1
		clothNodes[node2Id].linkDirections[2].push_back(clothLinkId);
	}
	else if (offsetFromP1ToP2.x < 0 && offsetFromP1ToP2.z < 0) {
		// North West link from 1 to 2
		linkDir = 7;
		clothNodes[node1Id].linkDirections[7].push_back(clothLinkId);
		// South East link from 2 to 1
		clothNodes[node2Id].linkDirections[3].push_back(clothLinkId);
	}

	// Add the new constraint to the constraint list
	SpringConstraint springConstraint(clothNodes[node1Id].pointMass, clothNodes[node2Id].pointMass, stiffness, breakDistance);
	clothLinks.emplace_back(node1Id, node2Id, linkDir, springConstraint);
}

ClothNode& ClothComponent::getNode(GLuint id)
{
	return clothNodes[id];
}

GLuint ClothComponent::getNumClothNodes() const
{
	return clothNodes.size();
}

ClothLink& ClothComponent::getClothLink(GLuint clothLinkId)
{
	return clothLinks.at(clothLinkId);
}

GLuint ClothComponent::getNumClothLinks() const
{
	return clothLinks.size();
}

GLuint ClothComponent::getNumPointMassesX() const
{
	return m_numPointMassesX;
}

GLuint ClothComponent::getNumPointMassesY() const
{
	return m_numPointMassesY;
}

void ClothComponent::breakStructualLink(GLuint clothLinkId)
{
	ClothLink& clothLink = clothLinks[clothLinkId];

	breakAllLinksFromNodeInDirection(clothLink.node1Id, wrapDirection(clothLink.direction - 1));
	breakAllLinksFromNodeInDirection(clothLink.node1Id, clothLink.direction);
	breakAllLinksFromNodeInDirection(clothLink.node1Id, wrapDirection(clothLink.direction  + 1));

	breakAllLinksFromNodeInDirection(clothLink.node2Id, getReverseDirection(wrapDirection(clothLink.direction - 1)));
	breakAllLinksFromNodeInDirection(clothLink.node2Id, getReverseDirection(clothLink.direction));
	breakAllLinksFromNodeInDirection(clothLink.node2Id, getReverseDirection(wrapDirection(clothLink.direction + 1)));
}

void ClothComponent::breakAllLinksFromNodeInDirection(GLuint nodeId, GLint direction)
{
	std::vector<GLuint>& linkIdsInDir = clothNodes[nodeId].linkDirections[direction];
	while (linkIdsInDir.size() > 0) {
		GLuint clothLinkId = linkIdsInDir[linkIdsInDir.size() - 1];
		
		// Solver should handle cleanup of broken links
		clothLinks[clothLinkId].springConstraint.isBroken = true;

		// TODO: Remove reverse link to prevent potential dangling references

		linkIdsInDir.pop_back();
	}
}

bool ClothComponent::hasConstraintBetween(GLuint idx1, GLuint idx2) const
{
	for (auto& linkDirs : clothNodes[idx1].linkDirections) {
		for (auto& clothLinkId : linkDirs) {
			if (clothLinks[clothLinkId].node2Id == idx2 || clothLinks[clothLinkId].node1Id == idx2)
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
	GLUtils::createTessellatedQuadData(numPointsX, numPointsY, width, height, vertices, cloth.triIndices);

	// Buffer triangle mesh to GPU for rendering
	clothEntity.model.rootNode.meshIDs.push_back(0);
	clothEntity.model.meshes.push_back(GLUtils::bufferMeshData(vertices, cloth.triIndices, GL_STREAM_DRAW));

	// Set Material to use for drawing
	Material material;
	material.willDrawWireframe = true;
	clothEntity.model.materials.push_back(material);

	// Create point masses from cloth vertices 
	cloth.clothNodes.reserve(vertices.size());
	for (GLuint i = 0; i < vertices.size(); ++i) {
		cloth.clothNodes.emplace_back(vertices[i].position, individualMass, false);

		// Fix first 4 verts
		if (i < numPointsX)
			cloth.clothNodes[i].pointMass.isFixed = true;
	}

	// Connect point masses with spring constraints
	for (GLuint i = 0; i < cloth.clothNodes.size(); ++i) {
		GLuint topLeftIdx = i;
		GLuint topLeftRow = topLeftIdx / cloth.m_numPointMassesX;
		GLuint topLeftCol = topLeftIdx % cloth.m_numPointMassesX;
		GLuint topRightIdx = i + 1;
		GLuint topRightRow = topLeftRow;
		GLuint topRightCol = topLeftCol + 1;
		GLuint bottomLeftIdx = i + cloth.m_numPointMassesX;
		GLuint bottomLeftRow = topLeftRow + 1;
		GLuint bottomLeftCol = topLeftCol;
		GLuint bottomRightIdx = i + cloth.m_numPointMassesX + 1;
		GLuint bottomRightRow = topLeftRow + 1;
		GLuint bottomRightCol = topLeftCol + 1;

		// Structural Constraints
		if (topRightCol < cloth.m_numPointMassesX)
			cloth.addClothLink(topLeftIdx, topRightIdx, 1.0f, 0.25f);
		if (bottomLeftRow < cloth.m_numPointMassesY)
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

GLint ClothComponent::wrapDirection(GLint direction)
{
	return (8 + direction) % 8;
}

GLint ClothComponent::getReverseDirection(GLint direction)
{
	return wrapDirection(direction + 4);
}

Entity& Prefabs::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight)
{
	return ClothComponent::createCloth(scene, numPointsX, numPointsY, width, height, totalWeight);
}

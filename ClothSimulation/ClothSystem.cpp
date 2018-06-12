#include "ClothSystem.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"
#include "Mesh.h"
#include "Clock.h"
#include "RenderSystem.h"

ClothSystem::ClothSystem(Scene& scene)
	: System(scene)
	, m_kNumConstraintSolverIterations{ 3 }
	, m_kDamping{ 0.01f }
	, m_kTimeStep{ 1.0f / 60.0f }
	, m_kTimeStepSq{ m_kTimeStep * m_kTimeStep }
	, windDirection{1.0f, 0.0f, 0.0f}
	, windForce(0.0f)
{
}


ClothSystem::~ClothSystem()
{
}

void ClothSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);
		
		// TODO: Add code for fixed time step

		if (entity.hasComponents(COMPONENT_CLOTH)) {
			ClothComponent& cloth = entity.cloth;

			// Solve constraints
			for (GLuint i = 0; i < m_kNumConstraintSolverIterations; ++i) {
				for (ClothLinkIterator clothLink = cloth.clothLinks.begin(); clothLink != cloth.clothLinks.end(); ++clothLink) {
					clothLink->springConstraint.solve();
				}
			}

			// Break constraints
			for (ClothLinkIterator clothLink = cloth.clothLinks.begin(); clothLink != cloth.clothLinks.end(); ++clothLink) {
				if ((clothLink->springConstraint.getCurrentLength() - clothLink->springConstraint.getRestLength()) >= clothLink->springConstraint.getBreakDistance()) {
					cloth.breakStructualLink(clothLink);
				}
			}

			// Cleanup broken links
			auto clothLinksIt = cloth.clothLinks.begin();
			while (clothLinksIt != cloth.clothLinks.end()) {
				if (clothLinksIt->springConstraint.isBroken) {
					clothLinksIt = cloth.clothLinks.erase(clothLinksIt);
				}
				else 
					++clothLinksIt;
			}

			// Add wind per triangle
			for (size_t i = 0; i < cloth.triIndices.size(); i += 3) {
				PointMass& point0 = cloth.clothNodes.at(cloth.triIndices.at(i)).pointMass;
				PointMass& point1 = cloth.clothNodes.at(cloth.triIndices.at(i + 1)).pointMass;
				PointMass& point2 = cloth.clothNodes.at(cloth.triIndices.at(i + 2)).pointMass;

				if (&point0 == &point1 && &point1 == &point2)
					continue;

				glm::vec3 side1 = point1.getPosition() - point0.getPosition();
				glm::vec3 side2 = point2.getPosition() - point0.getPosition();
				glm::vec3 normal = glm::normalize(glm::cross(side1, side2));

				glm::vec3 force = normal * glm::dot(normal, windDirection * windForce);

				point0.force += force;
				point1.force += force;
				point2.force += force;
			}

			// Integrate pointmasses
			for (GLuint i = 0; i < cloth.getNumClothNodes(); ++i) {
				ClothNode& clothNode = cloth.getNode(i);
				PointMass& pointMass = clothNode.pointMass;

				// Add gravity
				pointMass.force += pointMass.mass * glm::vec3{ 0, -9.81f, 0 };

				glm::vec3 tmp = pointMass.getPosition();
				glm::vec3 acceleration = pointMass.force / pointMass.mass;
				pointMass.addOffset((pointMass.getPosition() - pointMass.prevPosition) * (1.0f - m_kDamping) + acceleration * m_kTimeStepSq);
				pointMass.prevPosition = tmp;
				pointMass.force = { 0, 0, 0 };
			}

			// Map vertices to update on GPU
			glBindBuffer(GL_ARRAY_BUFFER, entity.model.meshes.at(0).VBO);
			auto vertices = static_cast<VertexFormat*>(glMapBufferRange(
				GL_ARRAY_BUFFER,
				0, 
				sizeof(VertexFormat) * cloth.getNumClothNodes(),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
			);

			// Map indices to update on GPU
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity.model.meshes.at(0).EBO);
			auto indices = static_cast<GLuint*>(glMapBufferRange(
				GL_ELEMENT_ARRAY_BUFFER,
				0,
				sizeof(GLuint) * 3 * 2 * (cloth.getNumPointMassesX() - 1) * (cloth.getNumPointMassesY() - 1),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
			);

			for (GLuint i = 0; i < cloth.getNumClothNodes(); ++i) {
				GLuint ptRow = (i / cloth.getNumPointMassesX());
				GLuint ptCol = (i % cloth.getNumPointMassesX());

				// Update GPU vertices to match cloth points
				vertices[i].position = cloth.getNode(i).pointMass.getPosition();
				vertices[i].normal = { 0, 1, 0 };
				vertices[i].texCoord = { 
					 ptCol / static_cast<float>(cloth.getNumPointMassesX() - 1), 
					 ptRow / static_cast<float>(cloth.getNumPointMassesY() - 1) 
				};

				// Update triangle indices
				if (ptRow < (cloth.getNumPointMassesY() - 1) && ptCol < (cloth.getNumPointMassesX() - 1)) {
					GLuint eboIdx = (ptRow * (cloth.getNumPointMassesX() - 1) + ptCol) * 6; // 6 Triangles per cloth patch

					GLuint topLeftIdx = i;
					GLuint topRightIdx = i + 1;
					GLuint bottomLeftIdx = i + cloth.getNumPointMassesX();
					GLuint bottomRightIdx = i + cloth.getNumPointMassesX() + 1;

					if (cloth.hasConstraintBetween(topLeftIdx, bottomLeftIdx) && cloth.hasConstraintBetween(bottomLeftIdx, bottomRightIdx)) {
						// Lower patch triangle
						cloth.triIndices[eboIdx] = indices[eboIdx] = topLeftIdx;
						cloth.triIndices[eboIdx + 1] = indices[eboIdx + 1] = bottomLeftIdx;
						cloth.triIndices[eboIdx + 2] = indices[eboIdx + 2] = bottomRightIdx;
					}
					else {
						// Degenerate triangle when link is broken
						indices[eboIdx] = indices[eboIdx + 1] = indices[eboIdx + 2] = topLeftIdx;
						cloth.triIndices[eboIdx] = cloth.triIndices[eboIdx + 1] = cloth.triIndices[eboIdx + 2] = topLeftIdx;
					}

					if (cloth.hasConstraintBetween(topLeftIdx, topRightIdx) && cloth.hasConstraintBetween(topRightIdx, bottomRightIdx)) {
						// Upper patch triangle
						cloth.triIndices[eboIdx + 3] = indices[eboIdx + 3] = topLeftIdx;
						cloth.triIndices[eboIdx + 4] = indices[eboIdx + 4] = bottomRightIdx;
						cloth.triIndices[eboIdx + 5] = indices[eboIdx + 5] = topRightIdx;
					}
					else {
						// Degenerate triangle when link is broken
						indices[eboIdx + 3] = indices[eboIdx + 4] = indices[eboIdx + 5] = topLeftIdx;
						cloth.triIndices[eboIdx + 3] = cloth.triIndices[eboIdx + 4] = cloth.triIndices[eboIdx + 5] = topLeftIdx;
					}
				}
			}

			// Unmap GPU buffers
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
	}
}

void ClothSystem::beginFrame()
{
}

void ClothSystem::endFrame()
{
}

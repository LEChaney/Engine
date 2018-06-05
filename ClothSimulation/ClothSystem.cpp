#include "ClothSystem.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"
#include "Mesh.h"
#include "Clock.h"

ClothSystem::ClothSystem(Scene& scene)
	: System(scene)
	, m_kNumConstraintSolverIterations{ 3 }
	, m_kDamping{ 0.01f }
	, m_kTimeStep{ 1.0f / 60.0f }
	, m_kTimeStepSq{ m_kTimeStep * m_kTimeStep }
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
				for (GLuint j = 0; j < cloth.springConstraints.size(); ++j) {
					auto constraintIt = cloth.springConstraints[j].begin();
					while (constraintIt != cloth.springConstraints[j].end()) {
						// Solve constraint
						bool broken;
						constraintIt->solveConstraint(broken);

						// Remove constraint if broken
						if (broken)
							constraintIt = cloth.springConstraints[j].erase(constraintIt);
						else
							++constraintIt;
					}
				}
			}

			for (PointMass& pointMass : cloth.pointMasses) {
				pointMass.force += pointMass.mass * glm::vec3{ 0, -9.81f, 0 };

				// Integrate pointmasses
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
				sizeof(VertexFormat) * cloth.pointMasses.size(),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
			);

			// Map indices to update on GPU
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity.model.meshes.at(0).EBO);
			auto indices = static_cast<GLuint*>(glMapBufferRange(
				GL_ELEMENT_ARRAY_BUFFER,
				0,
				sizeof(GLuint) * 3 * 2 * (cloth.numPointMassesX - 1) * (cloth.numPointMassesY - 1),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
			);

			for (GLuint i = 0; i < cloth.pointMasses.size(); ++i) {
				GLuint ptRow = (i / cloth.numPointMassesX);
				GLuint ptCol = (i % cloth.numPointMassesX);

				// Update GPU vertices to match cloth points
				vertices[i].position = cloth.pointMasses[i].getPosition();
				vertices[i].normal = { 0, 1, 0 };
				vertices[i].texCoord = { 
					 ptCol / static_cast<float>(cloth.numPointMassesX - 1), 
					 ptRow / static_cast<float>(cloth.numPointMassesY - 1) 
				};

				// Update triangle indices
				if (ptRow < (cloth.numPointMassesY - 1) && ptCol < (cloth.numPointMassesX - 1)) {
					GLuint eboIdx = (ptRow * (cloth.numPointMassesX - 1) + ptCol) * 6; // 6 Triangles per cloth patch

					GLuint topLeftIdx = i;
					GLuint topRightIdx = i + 1;
					GLuint bottomLeftIdx = i + cloth.numPointMassesX;
					GLuint bottomRightIdx = i + cloth.numPointMassesX + 1;

					if (cloth.hasConstraintBetween(topLeftIdx, bottomLeftIdx) && cloth.hasConstraintBetween(topLeftIdx, bottomRightIdx)) {
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

					if (cloth.hasConstraintBetween(topLeftIdx, bottomRightIdx) && cloth.hasConstraintBetween(topLeftIdx, topRightIdx)) {
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

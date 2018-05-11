#pragma once

#include <glad\glad.h>

#include <vector>

struct VertexFormat;
struct Mesh;
struct ModelComponent;

namespace GLPrimitives {
	// Returns the vertices to construct a quad.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getQuadVertices();

	// Returns the indices to construct a quad.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getQuadIndices();

	// Returns the vertices to construct a sphere.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getSphereVertices();

	// Returns the indices to construct a sphere.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<GLuint>& getSphereIndices();

	// Returns the vertices to construct a cylinder.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getCylinderVertices();

	// Returns the indices to construct a cylinder.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getCylinderIndices();

	// Returns the vertices to construct a pyramid.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getPyramidVertices();

	// Returns the indices to construct a pyramid.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getPyramidIndices();

	// Returns the vertices to construct a cube.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getCubeVertices();

	// Returns the indices to construct a cube.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getCubeIndices();

	// Returns a Mesh Component containing the VAO for a quad.
	// This function is cached for efficiency 
	// (Only 1 mesh will be buffered).
	const Mesh& getQuadMesh();

	// Returns a fully constructed and ready to render quad model.
	const ModelComponent& getQuadModel();

	// Returns a Mesh Component containing the VAO for a sphere.
	// This function is cached for efficiency 
	// (Only 1 mesh will be buffered).
	const Mesh& getSphereMesh();

	// Returns a fully constructed and ready to render quad model.
	const ModelComponent& getSphereModel();

	// Returns a Mesh Component containing the VAO for a cylinder.
	// This function is cached for efficiency 
	// (Only 1 mesh will be buffered).
	const Mesh& getCylinderMesh();

	// Returns a fully constructed and ready to render quad model.
	const ModelComponent& getCylinderModel();

	// Returns a Mesh Component containing the VAO for a pyramid.
	// This function is cached for efficiency
	// (Only 1 mesh will be buffered).
	const Mesh& getPyramidMesh();

	// Returns a fully constructed and ready to render quad model.
	const ModelComponent& getPyramidModel();

	// Returns a Mesh Component containing the VAO for a cube.
	// This function is cached for efficiency
	// (Only 1 mesh will be buffered).
	const Mesh& getCubeMesh();

	// Returns a fully constructed and ready to render quad model.
	const ModelComponent& getCubeModel();
}
#ifndef COMP3421_SHAPES_HPP
#define COMP3421_SHAPES_HPP

#include <glm/glm.hpp>
#include <ass3/mesh.hpp>

namespace shapes
{

	/**
	 * Creates a sphere static mesh with texture coordinates and initialises it
	 * @param tessellation - number of slices that make up the cube (must be divisible by 4)
	 */
	mesh::mesh_template_t make_sphere(float radius, unsigned int tessellation = 64);

	/**
	 * Creates a torus static mesh with texture coordinates and initialises it
	 * @param radius - radius of the entire torus
	 * @param thickness - radius of the torus rim
	 * @param tessellation - number of slices. The number of stacks should scale appropriately.
	 * @return
	 */
	mesh::mesh_template_t make_torus(float radius, float thickness, int tessellation = 64);

	mesh::mesh_template_t make_cube(float width);

	// assumes the mesh_template has indices
	void calc_vertex_normals(mesh::mesh_template_t &mesh_template);

	// assumes the mesh_template does not have indices
	void calc_face_normals(mesh::mesh_template_t &mesh_template);

	// duplicates any attributes based on the indices provided
	mesh::mesh_template_t expand_indices(const mesh::mesh_template_t &mesh_template);

	mesh::mesh_template_t make_ndc_rect();
} // namespace shapes
#endif // COMP3421_SHAPES_HPP

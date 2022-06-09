#include "ass3/scene.hpp"
#include "ass3/shapes.hpp"
#include "ass3/cubemap.hpp"
#include "ass3/renderer.hpp"
#include <iostream>

const char *EXTERIOR_PATH = "res/obj/exterior/exterior.obj";
const char *EXTERIOR_PORTAL_PATH = "res/obj/exterior/exterior_portal.obj";
const char *INTERIOR_PATH = "res/obj/interior/interior.obj";
const char *INTERIOR_PORTAL_PATH = "res/obj/interior/interior_portal.obj";
const char *SKYBOX_BASE_PATH = "res/skybox/sky";
const char *CAR_PATH = "res/obj/SPECTER_GT3_obj/SPECTER_GT3_.obj";
const char *SPACE_BASE_PATH = "res/spacebox/indigo";
const char *DUSK_BASE_PATH = "res/dusk/dusk";

namespace scene
{
	node_t make_scene_interior(GLuint portal_texture)
	{
		scene::node_t scene;

		scene::node_t interior;
		interior.kind = scene::node_t::STATIC_MESH;
		interior.model = model::load(INTERIOR_PATH);

		scene::node_t interior_portal;
		interior_portal.kind = scene::node_t::PORTAL;
		interior_portal.model = model::load(INTERIOR_PORTAL_PATH);
		interior_portal.model.materials[0].diffuse_map = portal_texture;
		interior_portal.model.materials[0].specular = glm::vec3(0);

		scene.children.push_back(interior);
		scene.children.push_back(interior_portal);

		return scene;
	}

	node_t make_scene_exterior(GLuint portal_texture)
	{
		scene::node_t scene;

		scene::node_t exterior;
		exterior.kind = scene::node_t::STATIC_MESH;
		// exterior.model = model::load(EXTERIOR_PATH);

		scene::node_t exterior_portal;
		exterior_portal.kind = scene::node_t::PORTAL;
		exterior_portal.model = model::load(EXTERIOR_PORTAL_PATH);
		exterior_portal.model.materials[0].diffuse_map = portal_texture;
		exterior_portal.model.materials[0].specular = glm::vec3(0);

		scene::node_t sphere;
		sphere.kind = scene::node_t::REFLECTIVE;
		auto sphere_mesh_template = shapes::make_sphere(1.0f);
		sphere.model.meshes.push_back(mesh::init(sphere_mesh_template));
		auto sphere_mat = model::material_t{};
		sphere_mat.specular = glm::vec3(0.7);
		sphere_mat.phong_exp = 500;
		sphere_mat.ambient = glm::vec3(10);
		sphere_mat.cube_map = cubemap::make_cubemap(SPACE_BASE_PATH);
		sphere.model.materials.push_back(sphere_mat);
		sphere.translation = glm::vec3(0, 5, -15);

		scene::node_t sphere2;
		sphere2.kind = scene::node_t::REFLECTIVE;
		auto sphere2_mesh_template = shapes::make_sphere(1.0f);
		sphere2.model.meshes.push_back(mesh::init(sphere2_mesh_template));
		auto sphere2_mat = model::material_t{};
		sphere2_mat.specular = glm::vec3(0.7);
		sphere2_mat.phong_exp = 500;
		sphere2_mat.ambient = glm::vec3(10);
		sphere2_mat.cube_map = cubemap::make_cubemap(DUSK_BASE_PATH);
		sphere2.model.materials.push_back(sphere2_mat);
		sphere2.scale = glm::vec3(2);
		sphere2.translation = glm::vec3(0, 10, -28);

		scene::node_t sphere3;
		sphere3.kind = scene::node_t::REFLECTIVE;
		auto sphere3_mesh_template = shapes::make_sphere(1.0f);
		sphere3.model.meshes.push_back(mesh::init(sphere3_mesh_template));
		auto sphere3_mat = model::material_t{};
		sphere3_mat.specular = glm::vec3(0.7);
		sphere3_mat.phong_exp = 500;
		sphere3_mat.ambient = glm::vec3(10);
		sphere3_mat.cube_map = cubemap::make_cubemap(SKYBOX_BASE_PATH);
		sphere3.model.materials.push_back(sphere3_mat);
		sphere3.scale = glm::vec3(1.5);
		sphere3.translation = glm::vec3(0, -5, -23);

		scene.children.push_back(exterior_portal);
		scene.children.push_back(exterior);
		scene.children.push_back(sphere);
		scene.children.push_back(sphere2);
		scene.children.push_back(sphere3);

		return scene;
	}

	model::model_t make_skybox()
	{
		auto skybox = model::model_t{};
		skybox.meshes.push_back(mesh::init(shapes::make_cube(1.0f)));
		skybox.materials.push_back({.cube_map = cubemap::make_cubemap(SKYBOX_BASE_PATH)});
		return skybox;
	}
} // namespace scene
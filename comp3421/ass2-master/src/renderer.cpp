#include "ass2/renderer.hpp"
#include "ass2/texture_2d.hpp"
#include "ass2/euler_camera.hpp"
#include "ass2/mesh.hpp"

#include "chicken3421/chicken3421.hpp"

const char *VERT_PATH = "res/shaders/shader.vert";
const char *FRAG_PATH = "res/shaders/shader.frag";

namespace renderer
{
	renderer_t init(const glm::mat4 &projection)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto renderer = renderer_t{};
		renderer.projection = projection;

		// make the render program
		auto vs = chicken3421::make_shader(VERT_PATH, GL_VERTEX_SHADER);
		auto fs = chicken3421::make_shader(FRAG_PATH, GL_FRAGMENT_SHADER);
		renderer.program = chicken3421::make_program(vs, fs);
		chicken3421::delete_shader(vs);
		chicken3421::delete_shader(fs);

		renderer.view_proj_loc = chicken3421::get_uniform_location(renderer.program, "uViewProj");
		renderer.model_loc = chicken3421::get_uniform_location(renderer.program, "uModel");
		renderer.camera_pos_loc = chicken3421::get_uniform_location(renderer.program, "uCameraPos");

		// TODO: sunlight uniform locations
		renderer.sun_light_dir_loc =
			chicken3421::get_uniform_location(renderer.program, "uSun.direction");
		renderer.sun_light_diffuse_loc =
			chicken3421::get_uniform_location(renderer.program, "uSun.diffuse");
		renderer.sun_light_ambient_loc =
			chicken3421::get_uniform_location(renderer.program, "uSun.ambient");

		// spotlight uniform locations
		renderer.spot_light_pos_loc[0] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[0].position");
		renderer.spot_light_diffuse_loc[0] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[0].diffuse");
		renderer.spot_light_ambient_loc[0] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[0].ambient");
		renderer.spot_light_specular_loc[0] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[0].specular");

		// the second plot light
		renderer.spot_light_pos_loc[1] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[1].position");
		renderer.spot_light_diffuse_loc[1] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[1].diffuse");
		renderer.spot_light_ambient_loc[1] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[1].ambient");
		renderer.spot_light_specular_loc[1] =
			chicken3421::get_uniform_location(renderer.program, "uSpot[1].specular");

		// TODO: material uniform locations
		renderer.mat_ambient_loc = chicken3421::get_uniform_location(renderer.program, "uMat.ambient");
		renderer.mat_diffuse_loc = chicken3421::get_uniform_location(renderer.program, "uMat.diffuse");
		renderer.mat_specular_loc =
			chicken3421::get_uniform_location(renderer.program, "uMat.specular");
		renderer.mat_phong_exp = chicken3421::get_uniform_location(renderer.program, "uMat.phongExp");

		// texture uniform locations
		renderer.diffuse_map_loc = chicken3421::get_uniform_location(renderer.program, "uDiffuseMap");
		renderer.diffuse_map_factor_loc =
			chicken3421::get_uniform_location(renderer.program, "uDiffuseMapFactor");
		renderer.specular_map_loc = chicken3421::get_uniform_location(renderer.program, "uSpecularMap");
		renderer.specular_map_factor_loc =
			chicken3421::get_uniform_location(renderer.program, "uSpecularMapFactor");

		return renderer;
	}

	void draw(const scene::node_t *node, const renderer_t &renderer, glm::mat4 model)
	{
		model *= glm::translate(glm::mat4(1.0), node->translation);
		model *= glm::rotate(glm::mat4(1.0), node->rotation.z, glm::vec3(0, 0, 1));
		model *= glm::rotate(glm::mat4(1.0), node->rotation.y, glm::vec3(0, 1, 0));
		model *= glm::rotate(glm::mat4(1.0), node->rotation.x, glm::vec3(1, 0, 0));
		model *= glm::scale(glm::mat4(1.0), node->scale);

		glUniformMatrix4fv(renderer.model_loc, 1, GL_FALSE, glm::value_ptr(model));

		for (auto i = size_t{0}; i < node->model.meshes.size(); ++i)
		{
			glUniform1f(renderer.diffuse_map_factor_loc,
						node->model.materials[i].diffuse_map ? 1.0f : 0.0f);
			glUniform1f(renderer.specular_map_factor_loc,
						node->model.materials[i].specular_map ? 1.0f : 0.0f);
			glUniform3fv(renderer.mat_ambient_loc, 1, glm::value_ptr(node->model.materials[i].ambient));
			glUniform4fv(renderer.mat_diffuse_loc, 1, glm::value_ptr(node->model.materials[i].diffuse));
			glUniform3fv(renderer.mat_specular_loc, 1,
						 glm::value_ptr(node->model.materials[i].specular));
			glUniform1f(renderer.mat_phong_exp, node->model.materials[i].phong_exp);
			glActiveTexture(GL_TEXTURE0);
			texture_2d::bind(node->model.materials[i].diffuse_map);
			glActiveTexture(GL_TEXTURE1);
			texture_2d::bind(node->model.materials[i].specular_map);

			mesh::draw(node->model.meshes[i]);
		}

		for (auto const &child : node->children)
		{
			draw(child, renderer, model);
		}
	}

	void render(const renderer_t &renderer, const scene::scene_t &scene)
	{
		glUseProgram(renderer.program);
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform3fv(renderer.camera_pos_loc, 1, glm::value_ptr(scene.camera.pos));

		glUniform3fv(renderer.sun_light_dir_loc, 1, glm::value_ptr(renderer.sun_light_dir));
		glUniform3fv(renderer.sun_light_diffuse_loc, 1, glm::value_ptr(renderer.sun_light_diffuse));
		glUniform3fv(renderer.sun_light_ambient_loc, 1, glm::value_ptr(renderer.sun_light_ambient));
		glUniform3fv(renderer.sun_light_specular_loc, 1,
					 glm::value_ptr(renderer.sun_light_specular));

		glUniform3fv(renderer.spot_light_pos_loc[0], 1, glm::value_ptr(renderer.spot_light_pos[0]));
		glUniform3fv(renderer.spot_light_diffuse_loc[0], 1,
					 glm::value_ptr(renderer.spot_light_diffuse[0]));
		glUniform3fv(renderer.spot_light_ambient_loc[0], 1,
					 glm::value_ptr(renderer.spot_light_ambient[0]));
		glUniform3fv(renderer.spot_light_specular_loc[0],
					 1, glm::value_ptr(renderer.spot_light_specular[0]));

		glUniform3fv(renderer.spot_light_pos_loc[1], 1, glm::value_ptr(renderer.spot_light_pos[1]));
		glUniform3fv(renderer.spot_light_diffuse_loc[1], 1,
					 glm::value_ptr(renderer.spot_light_diffuse[1]));
		glUniform3fv(renderer.spot_light_ambient_loc[1], 1,
					 glm::value_ptr(renderer.spot_light_ambient[1]));
		glUniform3fv(renderer.spot_light_specular_loc[1],
					 1, glm::value_ptr(renderer.spot_light_specular[1]));

		glUniform1i(renderer.diffuse_map_loc, 0);
		glUniform1i(renderer.specular_map_loc, 1);

		auto view_proj = renderer.projection * euler_camera::get_view(scene.camera);
		glUniformMatrix4fv(renderer.view_proj_loc, 1, GL_FALSE, glm::value_ptr(view_proj));

		draw(scene.root, renderer, glm::mat4(1.0f));
	}
} // namespace renderer

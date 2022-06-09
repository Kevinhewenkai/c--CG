#ifndef COMP3421_ASS2_RENDERER_HPP
#define COMP3421_ASS2_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ass2/scene.hpp"

namespace renderer
{
	struct renderer_t
	{
		glm::mat4 projection;

		// directional light attributes
		glm::vec3 sun_light_dir = glm::normalize(glm::vec3(0) - glm::vec3(50, 50, 0));
		glm::vec3 sun_light_diffuse = glm::vec3(1.0, 1.0, 0);
		glm::vec3 sun_light_ambient = glm::vec3(0.1f);
		glm::vec3 sun_light_specular = glm::vec3(1.0f);

		// spot light attributes
		glm::vec3 spot_light_pos[2] = {glm::vec3(-3, 1, 0), glm::vec3(5, 2, 1)};
		glm::vec3 spot_light_diffuse[2] = {glm::vec3(1.0f), glm::vec3(1.0, 0, 1.0)};
		glm::vec3 spot_light_ambient[2] = {glm::vec3(0.1f), glm::vec3(0.1f)};
		glm::vec3 spot_light_specular[2] = {glm::vec3(1.0f), glm::vec3(1.0f)};

		GLuint program;

		// vertex shader uniforms
		GLint view_proj_loc;
		GLint model_loc;
		GLint camera_pos_loc;

		// fragment directional light uniforms
		GLint sun_light_dir_loc;
		GLint sun_light_diffuse_loc;
		GLint sun_light_ambient_loc;
		GLint sun_light_specular_loc;

		// fragment directional light uniforms
		GLint spot_light_pos_loc[2];
		GLint spot_light_diffuse_loc[2];
		GLint spot_light_ambient_loc[2];
		GLint spot_light_specular_loc[2];

		// fragment material uniforms
		GLint mat_ambient_loc;
		GLint mat_diffuse_loc;
		GLint mat_specular_loc;
		GLint mat_phong_exp;

		// textures
		GLint diffuse_map_loc;
		GLint diffuse_map_factor_loc;
		GLint specular_map_loc;
		GLint specular_map_factor_loc;
	};

	renderer_t init(const glm::mat4 &projection);

	void draw(const scene::node_t *node, const renderer_t &renderer, glm::mat4 model);

	void render(const renderer_t &renderer, const scene::scene_t &scene);
} // namespace renderer

#endif // COMP3421_RENDERER_HPP

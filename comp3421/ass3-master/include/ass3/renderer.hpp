#ifndef COMP3421_RENDERER_HPP
#define COMP3421_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "scene.hpp"
#include "euler_camera.hpp"
#include "framebuffer.hpp"
#include "cubemap_camera.hpp"

namespace renderer
{
	struct renderer_t
	{
		glm::mat4 projection;

		GLuint program;
		GLuint hdr_program;
		GLuint skybox_program;
		GLuint post_processing_program;
		GLuint geometry_program;
		GLuint lighting_program;

		// directional light attributes
		glm::vec3 sun_light_dir = glm::normalize(glm::vec3(0) - glm::vec3(0, 50, -50));
		glm::vec3 sun_light_diffuse = glm::vec3(1.0f);
		glm::vec3 sun_light_ambient = glm::vec3(0.1f);
		glm::vec3 sun_light_specular = glm::vec3(1.0f);

		// spot light attributes
		glm::vec3 spot_light_pos = glm::vec3(0, 5, 5);
		glm::vec3 spot_light_diffuse = glm::vec3(1.0f);
		glm::vec3 spot_light_ambient = glm::vec3(0.1f);
		glm::vec3 spot_light_specular = glm::vec3(1.0f);

		glm::vec4 clip_plane = glm::vec4(0, 0, -1, -10.01);
	};

	renderer_t init(const glm::mat4 &projection);

	void toneMapping(const renderer_t &renderer, const framebuffer::framebuffer_t &framebuffer);

	void draw(scene::node_t &node, const renderer_t &renderer, glm::mat4 model);

	void render(const renderer_t &renderer,
				const euler_camera::camera_t &camera,
				const scene::node_t &scene,
				const model::model_t &skybox);

	void render(const renderer_t &renderer,
				const euler_camera::camera_t &camera,
				const scene::node_t &scene);

	GLuint renderEnvironmentMap(glm::vec3 position, double width, double height,
								renderer_t &renderer,
								scene::node_t &scene,
								model::model_t &skybox);

	void render_post_process(const renderer_t &renderer,
							 const framebuffer::framebuffer_t framebuffer,
							 GLuint rectVAO);

	scene::node_t make_portal(const renderer_t &renderer,
							  const scene::node_t &scene,
							  const model::model_t &skybox);
} // namespace renderer

#endif // COMP3421_RENDERER_HPP

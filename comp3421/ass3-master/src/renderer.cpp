#include "ass3/renderer.hpp"
#include "ass3/texture_2d.hpp"
#include "ass3/euler_camera.hpp"
#include "ass3/cubemap_camera.hpp"
#include "ass3/mesh.hpp"
#include "ass3/framebuffer.hpp"
#include "chicken3421/chicken3421.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <iostream>

const char *VERT_PATH = "res/shaders/shader.vert";
const char *FRAG_PATH = "res/shaders/shader.frag";
const char *HDR_VERT_PATH = "res/shaders/shader_hdr.vert";
const char *HDR_FRAG_PATH = "res/shaders/shader_hdr.frag";
const char *SKYBOX_VERT_PATH = "res/shaders/skybox.vert";
const char *SKYBOX_FRAG_PATH = "res/shaders/skybox.frag";

namespace renderer
{
	int locate(const std::string &name)
	{
		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		int loc = glGetUniformLocation(program, name.c_str());
		chicken3421::expect(loc != -1, "uniform not found: " + name);
		return loc;
	}

	void set_uniform(const std::string &name, float value)
	{
		glUniform1f(locate(name), value);
	}

	void set_uniform(const std::string &name, int value)
	{
		glUniform1i(locate(name), value);
	}

	void set_uniform(const std::string &name, glm::vec4 value)
	{
		glUniform4fv(locate(name), 1, glm::value_ptr(value));
	}

	void set_uniform(const std::string &name, glm::vec3 value)
	{
		glUniform3fv(locate(name), 1, glm::value_ptr(value));
	}

	void set_uniform(const std::string &name, const glm::mat4 &value)
	{
		glUniformMatrix4fv(locate(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	GLuint load_program(const std::string &vs_path, const std::string &fs_path)
	{
		GLuint vs = chicken3421::make_shader(vs_path, GL_VERTEX_SHADER);
		GLuint fs = chicken3421::make_shader(fs_path, GL_FRAGMENT_SHADER);
		GLuint handle = chicken3421::make_program(vs, fs);
		chicken3421::delete_shader(vs);
		chicken3421::delete_shader(fs);

		return handle;
	}

	renderer_t init(const glm::mat4 &projection)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto renderer = renderer_t{};
		renderer.projection = projection;

		// make the render program
		auto vs = chicken3421::make_shader(VERT_PATH, GL_VERTEX_SHADER);
		auto fs = chicken3421::make_shader(FRAG_PATH, GL_FRAGMENT_SHADER);
		renderer.program = chicken3421::make_program(vs, fs);
		chicken3421::delete_shader(vs);
		chicken3421::delete_shader(fs);

		auto hdr_vs = chicken3421::make_shader(HDR_VERT_PATH, GL_VERTEX_SHADER);
		auto hdr_fs = chicken3421::make_shader(HDR_FRAG_PATH, GL_FRAGMENT_SHADER);
		renderer.hdr_program = chicken3421::make_program(hdr_vs, hdr_fs);
		glUseProgram(renderer.hdr_program);
		set_uniform("hdrBuffer", 0);
		chicken3421::delete_shader(hdr_vs);
		chicken3421::delete_shader(hdr_fs);

		auto skybox_vs = chicken3421::make_shader(SKYBOX_VERT_PATH, GL_VERTEX_SHADER);
		auto skybox_fs = chicken3421::make_shader(SKYBOX_FRAG_PATH, GL_FRAGMENT_SHADER);
		renderer.skybox_program = chicken3421::make_program(skybox_vs, skybox_fs);
		chicken3421::delete_shader(skybox_vs);
		chicken3421::delete_shader(skybox_fs);

		return renderer;
	}

	void draw(const scene::node_t &node, const renderer_t &renderer, glm::mat4 model)
	{
		model *= glm::translate(glm::mat4(1.0), node.translation);
		model *= glm::rotate(glm::mat4(1.0), node.rotation.z, glm::vec3(0, 0, 1));
		model *= glm::rotate(glm::mat4(1.0), node.rotation.y, glm::vec3(0, 1, 0));
		model *= glm::rotate(glm::mat4(1.0), node.rotation.x, glm::vec3(1, 0, 0));
		model *= glm::scale(glm::mat4(1.0), node.scale);

		set_uniform("uModel", model);

		for (auto i = size_t{0}; i < node.model.meshes.size(); ++i)
		{
			set_uniform("uDiffuseMapFactor", node.model.materials[i].diffuse_map ? 1.0f : 0.0f);
			set_uniform("uSpecularMapFactor", node.model.materials[i].specular_map ? 1.0f : 0.0f);
			set_uniform("uCubeMapFactor", node.model.materials[i].cube_map ? 1.0f : 0.0f);
			set_uniform("uMat.ambient", node.model.materials[i].ambient);
			set_uniform("uMat.diffuse", node.model.materials[i].diffuse);
			set_uniform("uMat.specular", node.model.materials[i].specular);
			set_uniform("uMat.phongExp", node.model.materials[i].phong_exp);
			set_uniform("uIsPortal", node.kind == scene::node_t::PORTAL);
			glActiveTexture(GL_TEXTURE0);
			texture_2d::bind(node.model.materials[i].diffuse_map);
			glActiveTexture(GL_TEXTURE1);
			texture_2d::bind(node.model.materials[i].specular_map);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, node.model.materials[i].cube_map);
			mesh::draw(node.model.meshes[i]);
		}

		for (auto const &child : node.children)
		{
			draw(child, renderer, model);
		}
	}

	void draw_skybox(const model::model_t &model, const renderer_t &renderer, const glm::mat4 &view)
	{
		glUseProgram(renderer.skybox_program);
		glFrontFace(GL_CW);
		glDepthMask(GL_FALSE);

		set_uniform("uCubeMap", 0);
		set_uniform("uViewProj", renderer.projection * glm::mat4(glm::mat3(view)));
		for (auto i = size_t{0}; i < model.meshes.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, model.materials[i].cube_map);
			mesh::draw(model.meshes[i]);
		}
		glFrontFace(GL_CCW);
		glDepthMask(GL_TRUE);
		glUseProgram(0);
	}

	void render(const renderer_t &renderer,
				const euler_camera::camera_t &camera,
				const scene::node_t &scene,
				const model::model_t &skybox)
	{
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto view = euler_camera::get_view(camera);
		draw_skybox(skybox, renderer, view);

		glUseProgram(renderer.program);
		// glClearColor(0, 0, 0, 1.0);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		set_uniform("uCameraPos", camera.pos);

		set_uniform("uSun.direction", renderer.sun_light_dir);
		set_uniform("uSun.diffuse", renderer.sun_light_diffuse);
		set_uniform("uSun.ambient", renderer.sun_light_ambient);
		set_uniform("uSun.specular", renderer.sun_light_specular);

		set_uniform("uSpot.position", renderer.spot_light_pos);
		set_uniform("uSpot.diffuse", renderer.spot_light_diffuse);
		set_uniform("uSpot.ambient", renderer.spot_light_ambient);
		set_uniform("uSpot.specular", renderer.spot_light_specular);

		set_uniform("uDiffuseMap", 0);
		set_uniform("uSpecularMap", 1);
		set_uniform("uCubeMap", 2);

		set_uniform("uClipPlane", renderer.clip_plane);

		auto view_proj = renderer.projection * euler_camera::get_view(camera);
		set_uniform("uViewProj", view_proj);

		draw(scene, renderer, glm::mat4(1.0f));
	}

	void render(const renderer_t &renderer,
				const euler_camera::camera_t &camera,
				const scene::node_t &scene)
	{

		glUseProgram(renderer.program);
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		set_uniform("uCameraPos", camera.pos);

		set_uniform("uSun.direction", renderer.sun_light_dir);
		set_uniform("uSun.diffuse", renderer.sun_light_diffuse);
		set_uniform("uSun.ambient", renderer.sun_light_ambient);
		set_uniform("uSun.specular", renderer.sun_light_specular);

		set_uniform("uSpot.position", renderer.spot_light_pos);
		set_uniform("uSpot.diffuse", renderer.spot_light_diffuse);
		set_uniform("uSpot.ambient", renderer.spot_light_ambient);
		set_uniform("uSpot.specular", renderer.spot_light_specular);

		set_uniform("uDiffuseMap", 0);
		set_uniform("uSpecularMap", 1);
		set_uniform("uCubeMap", 2);

		set_uniform("uClipPlane", renderer.clip_plane);

		auto view_proj = renderer.projection * euler_camera::get_view(camera);
		set_uniform("uViewProj", view_proj);

		draw(scene, renderer, glm::mat4(1.0f));
	}

	void render(const renderer_t &renderer,
				const euler_camera::camera_t &camera,
				const scene::node_t &scene,
				const model::model_t &skybox,
				const glm::mat4 projection)
	{
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto view = euler_camera::get_view(camera);
		draw_skybox(skybox, renderer, view);

		glUseProgram(renderer.program);
		// glClearColor(0, 0, 0, 1.0);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		set_uniform("uCameraPos", camera.pos);

		set_uniform("uSun.direction", renderer.sun_light_dir);
		set_uniform("uSun.diffuse", renderer.sun_light_diffuse);
		set_uniform("uSun.ambient", renderer.sun_light_ambient);
		set_uniform("uSun.specular", renderer.sun_light_specular);

		set_uniform("uSpot.position", renderer.spot_light_pos);
		set_uniform("uSpot.diffuse", renderer.spot_light_diffuse);
		set_uniform("uSpot.ambient", renderer.spot_light_ambient);
		set_uniform("uSpot.specular", renderer.spot_light_specular);

		set_uniform("uDiffuseMap", 0);
		set_uniform("uSpecularMap", 1);
		set_uniform("uCubeMap", 2);

		set_uniform("uClipPlane", renderer.clip_plane);

		auto view_proj = projection * euler_camera::get_view(camera);
		set_uniform("uViewProj", view_proj);

		draw(scene, renderer, glm::mat4(1.0f));
	}

	void toneMapping(const renderer_t &renderer, const framebuffer::framebuffer_t &framebuffer)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(renderer.hdr_program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
		set_uniform("hdr", true);
		set_uniform("exposure", 1.0f);
		// TODO: may have a problem
		GLuint quadVAO = 0;
		GLuint quadVBO;
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,
				1.0f,
				0.0f,
				0.0f,
				1.0f,
				-1.0f,
				-1.0f,
				0.0f,
				0.0f,
				0.0f,
				1.0f,
				1.0f,
				0.0f,
				1.0f,
				1.0f,
				1.0f,
				-1.0f,
				0.0f,
				1.0f,
				0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	// void renderscene(const renderer_t &renderer,
	// 				 const cubemap_camera::cubemap_camera_t &camera,
	// 				 const scene::node_t &scene,
	// 				 model::model_t &skybox)
	// {

	// 	glUseProgram(renderer.skybox_program);
	// 	glClearColor(0, 0, 0, 1.0);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 	glFrontFace(GL_CW);
	// 	glDepthMask(GL_FALSE);

	// 	set_uniform("uCubeMap", 0);
	// 	set_uniform("uViewProj", camera.proj * camera.view);
	// 	for (auto i = size_t{0}; i < skybox.meshes.size(); ++i)
	// 	{
	// 		glActiveTexture(GL_TEXTURE0);
	// 		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.materials[i].cube_map);
	// 		mesh::draw(skybox.meshes[i]);
	// 	}
	// 	glFrontFace(GL_CCW);
	// 	glDepthMask(GL_TRUE);
	// 	glUseProgram(0);

	// 	// render the scene
	// 	glUseProgram(renderer.program);
	// 	glClearColor(0, 0, 0, 1.0);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 	set_uniform("uCameraPos", camera.pos);

	// 	set_uniform("uSun.direction", renderer.sun_light_dir);
	// 	set_uniform("uSun.diffuse", renderer.sun_light_diffuse);
	// 	set_uniform("uSun.ambient", renderer.sun_light_ambient);
	// 	set_uniform("uSun.specular", renderer.sun_light_specular);

	// 	set_uniform("uSpot.position", renderer.spot_light_pos);
	// 	set_uniform("uSpot.diffuse", renderer.spot_light_diffuse);
	// 	set_uniform("uSpot.ambient", renderer.spot_light_ambient);
	// 	set_uniform("uSpot.specular", renderer.spot_light_specular);

	// 	set_uniform("uDiffuseMap", 0);
	// 	set_uniform("uSpecularMap", 1);
	// 	set_uniform("uCubeMap", 2);

	// 	set_uniform("uClipPlane", renderer.clip_plane);

	// 	auto view_proj = camera.proj * camera.view;
	// 	set_uniform("uViewProj", view_proj);

	// 	draw(scene, renderer, glm::mat4(1.0f));

	// 	//render the skybox
	// }

	GLuint renderEnvironmentMap(glm::vec3 position, double width, double height, renderer_t &renderer, scene::node_t &scene, model::model_t &skybox)
	{
		GLuint color_tex, fb, depth_rb;
		glm::mat4 shadowProj = glm::perspective(glm::radians(45.0), (double)height / (double)height, 0.1, 1000.0);
		//RGBA8 Cubemap texture, 24 bit depth texture, 256x256
		glGenTextures(1, &color_tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, color_tex);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//NULL means reserve texture memory, but texels are undefined
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, 0, GL_RGB, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, 0, GL_RGB, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2, 0, GL_RGB, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3, 0, GL_RGB, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, GL_RGB, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, GL_RGB, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		//-------------------------
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		//Attach one of the faces of the Cubemap texture to this FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, color_tex, 0);
		//-------------------------
		glGenRenderbuffers(1, &depth_rb);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 256, 256);
		//-------------------------
		//Attach depth buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
		//-------------------------
		euler_camera::camera_t camera = euler_camera::make_camera(position, position + glm::vec3(1.0f, 0.0f, 0.0f));
		render(renderer, camera, scene, skybox);
		//In order to render to the other faces, do this :
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, color_tex, 0);
		euler_camera::camera_t camera1 = euler_camera::make_camera(position, position + glm::vec3(-1.0f, 0.0f, 0.0f));
		render(renderer, camera1, scene, skybox, shadowProj);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, color_tex, 0);
		euler_camera::camera_t camera2 = euler_camera::make_camera(position, position + glm::vec3(0.0f, 1.0f, 0.0f));
		render(renderer, camera2, scene, skybox, shadowProj);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y + 1, color_tex, 0);
		euler_camera::camera_t camera3 = euler_camera::make_camera(position, position + glm::vec3(0.0f, -1.0f, 0.0f));
		render(renderer, camera3, scene, skybox, shadowProj);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, color_tex, 0);
		euler_camera::camera_t camera4 = euler_camera::make_camera(position, position + glm::vec3(0.0f, 0.0f, 1.0f));
		render(renderer, camera4, scene, skybox, shadowProj);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z + 1, color_tex, 0);
		euler_camera::camera_t camera5 = euler_camera::make_camera(position, position + glm::vec3(0.0f, 0.0f, -1.0f));
		render(renderer, camera5, scene, skybox, shadowProj);
		//... and so on
		//Delete resources
		// glDeleteTextures(1, &color_tex);
		glDeleteRenderbuffers(1, &depth_rb);
		//Bind 0, which means render to back buffer, as a result, fb is unbound
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fb);
		return color_tex;
	}

	GLuint magic_camera(const renderer_t &renderer,
						const scene::node_t &scene,
						const model::model_t &skybox)
	{
		framebuffer::framebuffer_t framebuffer = framebuffer::make_post_framebuffer(1280, 720);
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0), (double)720.0 / (double)720.0, 0.1, 1000.0);
		glm::vec3 position = glm::vec3(0, 5, 20);
		const euler_camera::camera_t camera = euler_camera::make_camera(position, position + glm::vec3(-1.0f, 0.0f, 0.0f));
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		render(renderer, camera, scene, skybox);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return framebuffer.texture;
	}

	scene::node_t make_portal(const renderer_t &renderer,
							  const scene::node_t &scene,
							  const model::model_t &skybox)
	{
		scene::node_t exterior_portal;
		exterior_portal.translation = glm::vec3(0, 5, 15);
		exterior_portal.kind = scene::node_t::PORTAL;
		exterior_portal.model = model::load("res/obj/exterior/exterior_portal.obj");
		GLuint portal_texture = magic_camera(renderer, scene, skybox);
		exterior_portal.model.materials[0].diffuse_map = portal_texture;
		exterior_portal.model.materials[0].specular = glm::vec3(0);

		return exterior_portal;
	}
} // namespace renderer

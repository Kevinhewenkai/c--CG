#include <thread>
#include <chrono>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>

#include <chicken3421/chicken3421.hpp>

#include "ass3/texture_2d.hpp"
#include "ass3/shapes.hpp"
#include "ass3/euler_camera.hpp"
#include "ass3/memes.hpp"
#include "ass3/renderer.hpp"
#include "ass3/animation.hpp"
#include "ass3/framebuffer.hpp"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

const char *WIN_TITLE = "ass 3";

/**
 * Returns the difference in time between when this function was previously called and this call.
 * @return A float representing the difference between function calls in seconds.
 */
float time_delta();

/**
 * Returns the current time in seconds.
 * @return Returns the current time in seconds.
 */
float time_now();

namespace renderer_2d
{
    const char *VERT_PATH = "res/shaders/shader2d.vert";
    const char *FRAG_PATH = "res/shaders/shader2d.frag";

    const char *VERT_PATH_2 = "res/shaders/shader2d_2.vert";
    const char *FRAG_PATH_2 = "res/shaders/shader2d_2.frag";
    struct renderer_t
    {
        GLuint kernel_program;
        GLuint second_program;
    };

    renderer_t init()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto renderer = renderer_t{};

        // make the render program
        auto vs = chicken3421::make_shader(renderer_2d::VERT_PATH, GL_VERTEX_SHADER);
        auto fs = chicken3421::make_shader(renderer_2d::FRAG_PATH, GL_FRAGMENT_SHADER);
        renderer.kernel_program = chicken3421::make_program(vs, fs);
        chicken3421::delete_shader(vs);
        chicken3421::delete_shader(fs);

        auto vs2 = chicken3421::make_shader(renderer_2d::VERT_PATH_2, GL_VERTEX_SHADER);
        auto fs2 = chicken3421::make_shader(renderer_2d::FRAG_PATH_2, GL_FRAGMENT_SHADER);
        renderer.second_program = chicken3421::make_program(vs2, fs2);
        chicken3421::delete_shader(vs2);
        chicken3421::delete_shader(fs2);

        return renderer;
    }

    void render(renderer_t renderer, mesh::mesh_t canvas, GLuint diffuse_map)
    {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(renderer.kernel_program);
        chicken3421::set_uniform(renderer.kernel_program, "uDiffuseMap", 0);
        glActiveTexture(GL_TEXTURE0);
        texture_2d::bind(diffuse_map);
        mesh::draw(canvas);
    }

    void render2(renderer_t renderer, mesh::mesh_t canvas, GLuint diffuse_map)
    {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(renderer.kernel_program);
        chicken3421::set_uniform(renderer.second_program, "uDiffuseMap", 0);
        glActiveTexture(GL_TEXTURE0);
        texture_2d::bind(diffuse_map);
        mesh::draw(canvas);
    }
} // namespace renderer_2d

int main()
{
#ifndef __APPLE__
    chicken3421::enable_debug_output();
#endif
    GLFWwindow *window = marcify(chicken3421::make_opengl_window(SCR_WIDTH, SCR_HEIGHT, WIN_TITLE));
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // // Enables Multisampling
    glEnable(GL_MULTISAMPLE);

    // auto canvas = mesh::init(shapes::make_ndc_rect());
    bool first_post_process = false;
    bool second_post_process = false;

    auto skybox = scene::make_skybox();

    auto camera = euler_camera::make_camera({7.5, 10, -22.5}, {0, 5, -15});
    auto renderer = renderer::init(
        glm::perspective(glm::radians(60.0), (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.1, 1000.0));

    // TODO: create framebuffer here
    auto framebuffer = framebuffer::make_post_framebuffer(SCR_WIDTH, SCR_HEIGHT);
    auto framebuffer2 = framebuffer::make_post_framebuffer(SCR_WIDTH, SCR_HEIGHT);
    auto anti_1 = framebuffer::make_framebuffer(SCR_WIDTH, SCR_HEIGHT);
    auto anti_2 = framebuffer::make_framebuffer(SCR_WIDTH, SCR_HEIGHT);
    auto anti_3 = framebuffer::make_framebuffer(SCR_WIDTH, SCR_HEIGHT);
    auto hdrFramebuffer = framebuffer::make_post_hdr_framebuffer(SCR_WIDTH, SCR_HEIGHT);
    // bind the framebuffer texture to the portal
    auto exterior = scene::make_scene_exterior(hdrFramebuffer.texture);
    auto interior = scene::make_scene_interior(hdrFramebuffer.texture);

    auto canvas = mesh::init(shapes::make_ndc_rect());

    auto renderer_2d = renderer_2d::init();

    bool camera_in_interior = false;
    // set up the dynamic env mapping sphere
    scene::node_t sphere1;
    sphere1.kind = scene::node_t::REFLECTIVE;
    auto sphere1_mesh_template = shapes::make_sphere(1.0f);
    sphere1.model.meshes.push_back(mesh::init(sphere1_mesh_template));
    auto sphere1_mat = model::material_t{};
    sphere1_mat.specular = glm::vec3(0.7);
    sphere1_mat.phong_exp = 0.1;
    sphere1_mat.ambient = glm::vec3(10);
    // get the cubemap by a temp camera
    sphere1_mat.cube_map = renderer::renderEnvironmentMap(glm::vec3(0, 5, -20), SCR_WIDTH, SCR_HEIGHT, renderer, exterior, skybox);
    sphere1.model.materials.push_back(sphere1_mat);
    sphere1.translation = glm::vec3(0, 5, -20);
    exterior.children.push_back(sphere1);

    // the plot light to test hdr
    auto light_cube = scene::node_t{};
    light_cube.model.meshes.push_back(mesh::init(shapes::make_cube(0.1f)));
    light_cube.model.materials.push_back({.ambient = glm::vec3(500)});
    light_cube.translation = renderer.spot_light_pos;
    interior.children.push_back(light_cube);

    auto portal = renderer::make_portal(renderer, exterior, skybox);
    exterior.children.push_back(portal);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        {
            first_post_process = true;
        }

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            second_post_process = true;
        }

        auto dt = (float)time_delta();

        glm::vec3 f = camera.pos + glm::vec3(get_view(camera) * glm::vec4(0, 0, -0.2, 0));
        if (f.x > -1.3f && f.x < 1.3f && f.y > 2.5f && f.y < 7.5f && f.z < -9.5f && f.z > -10.5f)
        {
            if (camera_in_interior && f.z < -10)
            {
                camera_in_interior = false;
            }
            else if (!camera_in_interior && f.z > -10)
            {
                camera_in_interior = true;
            }
        }

        euler_camera::update_camera(camera, window, dt);

        // render normal scene
        if (camera_in_interior)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, anti_1.fbo);
            glEnable(GL_CLIP_DISTANCE0);
            renderer::render(renderer, camera, exterior, skybox);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, anti_1.fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFramebuffer.fbo);
            glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_CLIP_DISTANCE0);

            renderer::toneMapping(renderer, hdrFramebuffer);

            // TODO
            if (first_post_process && second_post_process)
            {

                glBindFramebuffer(GL_FRAMEBUFFER, anti_2.fbo);
                renderer::render(renderer, camera, interior);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, anti_2.fbo);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.fbo);
                glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, anti_3.fbo);
                renderer_2d::render(renderer_2d, canvas, framebuffer.texture);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, anti_3.fbo);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer2.fbo);
                glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                renderer_2d::render2(renderer_2d, canvas, framebuffer2.texture);

                animation::animate(interior, dt);
            }
            else if (first_post_process)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, anti_2.fbo);
                renderer::render(renderer, camera, interior);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, anti_2.fbo);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.fbo);
                glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                renderer_2d::render(renderer_2d, canvas, framebuffer.texture);
            }
            else if (second_post_process)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, anti_2.fbo);
                renderer::render(renderer, camera, interior);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, anti_2.fbo);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.fbo);
                glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                renderer_2d::render2(renderer_2d, canvas, framebuffer.texture);
            }
            else
            {
                renderer::render(renderer, camera, interior);
            }
        }
        else
        {
            sphere1_mat.cube_map = renderer::renderEnvironmentMap(glm::vec3(0, 5, -20), SCR_WIDTH, SCR_HEIGHT, renderer, exterior, skybox);
            // render the portal
            glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebuffer.fbo);
            renderer::render(renderer, camera, interior);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            renderer::toneMapping(renderer, hdrFramebuffer);

            renderer::render(renderer, camera, exterior, skybox);

            animation::animate(exterior, dt);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

float time_delta()
{
    static float then = time_now();
    float now = time_now();
    float dt = now - then;
    then = now;
    return dt;
}

float time_now()
{
    return (float)glfwGetTime();
}

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

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;
// - Colors
// const GLuint NR_LIGHTS = 32;
// std::vector<glm::vec3> lightPositions;
// std::vector<glm::vec3> lightColors;
// for (GLuint i = 0; i < 32; i++)
// {
//     // Calculate slightly random offsets
//     GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//     GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
//     GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//     lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
//     // Also calculate random color
//     GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
//     GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
//     GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
//     lightColors.push_back(glm::vec3(rColor, gColor, bColor));
// }
namespace defer_renderer
{

    struct buffer
    {
        GLuint gBuffer;
        GLuint gPosition;
        GLuint gNormal;
        GLuint gAlbedoSpec;
    };

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

    buffer make_gbuffer()
    {
        // Set up G-Buffer
        // 3 textures:
        // 1. Positions (RGB)
        // 2. Color (RGB) + Specular (A)
        // 3. Normals (RGB)
        GLuint gBuffer;
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        GLuint gPosition, gNormal, gAlbedoSpec;
        // - Position color buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        // - Normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        // - Color + Specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
        // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);
        // - Create and attach depth buffer (renderbuffer)
        GLuint rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // - Finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return {gBuffer, gPosition, gNormal, gAlbedoSpec};
    }

    void draw(const scene::node_t &node, const renderer::renderer_t &renderer, glm::mat4 model)
    {
        model *= glm::translate(glm::mat4(1.0), node.translation);
        model *= glm::rotate(glm::mat4(1.0), node.rotation.z, glm::vec3(0, 0, 1));
        model *= glm::rotate(glm::mat4(1.0), node.rotation.y, glm::vec3(0, 1, 0));
        model *= glm::rotate(glm::mat4(1.0), node.rotation.x, glm::vec3(1, 0, 0));
        model *= glm::scale(glm::mat4(1.0), node.scale);

        set_uniform("uModel", model);

        for (auto i = size_t{0}; i < node.model.meshes.size(); ++i)
        {
            mesh::draw(node.model.meshes[i]);
        }

        for (auto const &child : node.children)
        {
            draw(child, renderer, model);
        }
    }

    void render(const renderer::renderer_t &renderer,
                const euler_camera::camera_t &camera,
                const scene::node_t &scene,
                const buffer buffer)
    {
        // 1. Geometry Pass: render scene's geometry/color data into gbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, buffer.gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(60.0), (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.1, 1000.0);
        glm::mat4 view = euler_camera::get_view(camera);
        glm::mat4 model;
        glUseProgram(renderer.geometry_program);
        set_uniform("uViewProj", projection * view);
        draw(scene, renderer, model);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(renderer.lighting_program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer.gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, buffer.gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, buffer.gAlbedoSpec);
    }
}
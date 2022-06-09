// C++17 doesn't have std::pi
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include <cstdlib>
#include <chrono>
#include <stb/stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chicken3421/chicken3421.hpp>

struct image_t
{
    int width;
    int height;
    int n_channels;
    void *data;
};
struct vertex_t
{
    glm::vec4 pos;
    glm::vec2 tc;
};

struct rect_t
{
    GLuint vao;
    GLuint vbo;
    std::vector<vertex_t> vertices;

    glm::mat4 t;
    glm::mat4 r;
    glm::mat4 s;
};

image_t load_image(const std::string &filename)
{
    image_t img; // NOLINT(cppcoreguidelines-pro-type-member-init)

    stbi_set_flip_vertically_on_load(true);
    img.data = stbi_load(filename.data(), &img.width, &img.height, &img.n_channels, 0);

    chicken3421::expect(img.data, "Could not read " + filename);

    return img;
}

void delete_image(image_t &img)
{
    stbi_image_free(img.data);

    img.data = nullptr;
    img.width = 0;
    img.height = 0;
    img.n_channels = 0;
}

GLuint make_texture(const image_t &tex_img)
{
    GLint format = tex_img.n_channels == 3 ? GL_RGB : GL_RGBA; // assuming that if not 3 channels then 4

    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex_img.width, tex_img.height, 0, format, GL_UNSIGNED_BYTE, tex_img.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

rect_t make_rect(float xmin, float ymin, float xmax, float ymax)
{
    rect_t rectangle;

    std::vector<vertex_t> verts = {
        {{xmax, ymax, 0, 1}, {1, 1}}, // top-right
        {{xmax, ymin, 0, 1}, {1, 0}}, // bottom-right
        {{xmin, ymin, 0, 1}, {0, 0}}, // bottom-left

        {{xmax, ymax, 0, 1}, {1, 1}}, // top-right
        {{xmin, ymin, 0, 1}, {0, 0}}, // bottom-left
        {{xmin, ymax, 0, 1}, {0, 1}}, // top-left
    };

    // create a vao
    GLuint vao;
    glGenVertexArrays(1, &vao);

    rectangle.vertices = verts;
    rectangle.vao = vao;

    rectangle.t = glm::identity<glm::mat4>();
    rectangle.r = glm::identity<glm::mat4>();
    rectangle.s = glm::identity<glm::mat4>();

    GLuint vbo;
    glGenBuffers(1, &vbo);

    rectangle.vbo = vbo;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, (GLintptr)(sizeof(vertex_t) * verts.size()), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)(0 + offsetof(vertex_t, pos)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)(0 + offsetof(vertex_t, tc)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return rectangle;
}

GLint get_uniform_location(GLuint program, const std::string &name)
{
    GLint loc = glGetUniformLocation(program, name.data());
    chicken3421::expect(loc != -1, "No uniform variable named: " + name + "in program: " + std::to_string(loc));
    return loc;
}

void delete_rectangle(rect_t &r)
{
    r.vertices.clear();
    glDeleteBuffers(1, &r.vbo);
    glDeleteVertexArrays(1, &r.vao);
}

void delete_texture(GLuint tex)
{
    glDeleteTextures(1, &tex);
}

void on_mouse_click(GLFWwindow *win, int button, int action, int mods)
{
    rect_t *mesh = (rect_t *)glfwGetWindowUserPointer(win);

    mesh->t = glm::translate(mesh->t, glm::vec3{-0.1, -0.1, 0});
}

int main()
{
    GLFWwindow *win = chicken3421::make_opengl_window(1280, 720, "Assignment 1");
    chicken3421::make_opengl_context(win);

    // create rectangles
    rect_t mesh = make_rect(0.2, 0.2, 0.5, 0.5);
    rect_t mesh2 = make_rect(-0.2, -0.2, 0.2, 0.2);
    rect_t mesh3 = make_rect(-1, -1, 1, 1);

    // create shader
    GLuint vs = chicken3421::make_shader("res/shaders/vert.glsl", GL_VERTEX_SHADER);
    GLuint fs = chicken3421::make_shader("res/shaders/frag.glsl", GL_FRAGMENT_SHADER);
    GLuint render_prog = chicken3421::make_program(vs, fs);

    // // refer the  m_transfrom, loc_tex
    GLint m_transform = get_uniform_location(render_prog, "m_transform");
    GLint loc_tex = glGetUniformLocation(render_prog, "tex");

    // // make texture
    image_t pokemon = load_image("res/img/pokemon.png");
    GLuint tex1 = make_texture(pokemon);

    GLint ball_m_transform = get_uniform_location(render_prog, "m_transform");
    GLint ball_loc_tex = glGetUniformLocation(render_prog, "tex");

    image_t pokemon_ball = load_image("res/img/pokemon_ball.png");
    GLuint tex2 = make_texture(pokemon_ball);

    image_t background = load_image("res/img/background.png");
    GLuint tex3 = make_texture(background);

    GLint bg_m_transform = get_uniform_location(render_prog, "m_transform");
    GLint bg_loc_tex = glGetUniformLocation(render_prog, "tex");

    int angle = 1;
    int direction = 1;
    int time = 0;

    // TODO: call setup
    glfwSetMouseButtonCallback(win, on_mouse_click);
    glfwSetWindowUserPointer(win, &mesh);

    while (!glfwWindowShouldClose(win))
    {

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(255, 255, 255, 1);

        glUseProgram(render_prog);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //background
        glBindVertexArray(mesh3.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh3.vbo);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(bg_loc_tex, 0);
        glBindTexture(GL_TEXTURE_2D, tex3);

        glUniformMatrix4fv(bg_m_transform, 1, GL_FALSE, glm::value_ptr(mesh3.r * mesh3.s * mesh3.t));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // first rectangle
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(loc_tex, 1);
        glBindTexture(GL_TEXTURE_2D, tex1);

        glUniformMatrix4fv(m_transform, 1, GL_FALSE, glm::value_ptr(mesh.r * mesh.s * mesh.t));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // second rectangle
        glBindVertexArray(mesh2.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh2.vbo);
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(ball_loc_tex, 2);
        glBindTexture(GL_TEXTURE_2D, tex2);

        glUniformMatrix4fv(ball_m_transform, 1, GL_FALSE, glm::value_ptr(mesh2.r * mesh2.s * mesh2.t));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // mesh.t = glm::translate(mesh.t, glm::vec3(-0.5, 0, 0));
        // glUniformMatrix4fv(ball_m_transform, 1, GL_FALSE, glm::value_ptr(mesh.t * mesh.r * mesh.s));
        // glDrawArrays(GL_TRIANGLES, 0, 6);

        glUseProgram(0);

        if (angle % 30 == 0)
        {
            direction = -1 * direction;
        }
        if (time < 1500)
        {
            mesh2.r = glm::rotate(mesh2.r, glm::radians(1.0f * (float)direction), glm::vec3(0.0f, 0.0f, 1.0f));
            angle += 1 * direction;
            time++;
        }
        mesh.s = glm::scale(mesh.s, glm::vec3(0.999, 0.999, 0));
        glfwSwapBuffers(win);
    }
    delete_rectangle(mesh);
    delete_rectangle(mesh2);
    delete_texture(tex1);
    delete_texture(tex2);
    delete_image(pokemon);
    delete_image(pokemon_ball);
    chicken3421::delete_program(render_prog);
    chicken3421::delete_shader(fs);
    chicken3421::delete_shader(vs);
    glfwDestroyWindow(win);

    return EXIT_SUCCESS;
}
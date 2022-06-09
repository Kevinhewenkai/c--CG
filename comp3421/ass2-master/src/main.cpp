#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <chicken3421/chicken3421.hpp>

#include <ass2/memes.hpp>
#include "ass2/texture_2d.hpp"
#include "ass2/shapes.hpp"
#include "ass2/euler_camera.hpp"
#include "ass2/renderer.hpp"
#include "ass2/model.hpp"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

const char *AIRCRAFT_OBJ_PATH = "res/obj/aircraft/aircraft.obj";
const char *TRACK_PATH = "res/obj/race_scene/race_scene.obj";
const char *CAR_PATH = "res/obj/SPECTER_GT3_obj/SPECTER_GT3_.obj";

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

bool specialView = false;

// design the bezier curve for the movement of aircraft
glm::vec3 cubic_bezier(const std::vector<glm::vec3> &control_points, float t)
{
    glm::vec3 pos = (1 - t) * (1 - t) * (1 - t) * control_points[0] + 3 * t * (1 - t) * (1 - t) * control_points[1] + 3 * t * t * (1 - t) * control_points[2] + t * t * t * control_points[3];
    return pos;
}

void update_light_cube(GLFWwindow *window, scene::node_t *cube, float dt)
{
    static float angle = glm::radians(45.0);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        angle += dt;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        angle -= dt;
    }
    float radius = 5;
    cube->translation.x = radius * glm::cos(angle);
    cube->translation.y = radius * glm::sin(angle);
}

void special_view(GLFWwindow *window, euler_camera::camera_t &cam, scene::node_t node)
{
    cam.pos = glm::vec3(node.translation.x, node.translation.y + 20, node.translation.z);
}

int main()
{
    GLFWwindow *win = marcify(chicken3421::make_opengl_window(1280, 720, "Assignment 2"));

    // track -> largeAircraft
    //       -> car
    //       -> plot light
    // largeAircraft -> medium aircraft -> smallCraft
    // load the root
    auto track = scene::node_t{};
    track.model = model::load(TRACK_PATH);
    for (auto &mat : track.model.materials)
    {
        mat.specular = glm::vec3(0);
    }

    // load the large aircraft
    auto largeAircraft = scene::node_t{};
    largeAircraft.model = model::load(AIRCRAFT_OBJ_PATH);
    largeAircraft.scale *= 0.03;
    track.children.push_back(&largeAircraft);

    // load sub aircraft
    auto mediumAircraft1 = scene::node_t{};
    mediumAircraft1.model = model::load(AIRCRAFT_OBJ_PATH);
    mediumAircraft1.scale *= 0.5;
    mediumAircraft1.translation = glm::vec3(100.0f, -10.0f, -10.0f);
    largeAircraft.children.push_back(&mediumAircraft1);

    // load sub aircraft
    auto mediumAircraft2 = scene::node_t{};
    mediumAircraft2.model = model::load(AIRCRAFT_OBJ_PATH);
    mediumAircraft2.scale *= 0.5;
    mediumAircraft2.translation = glm::vec3(-100.0f, -10.0f, -10.0f);
    largeAircraft.children.push_back(&mediumAircraft2);

    // load sub sub aircraft
    auto smallAircraft1 = scene::node_t{};
    smallAircraft1.model = model::load(AIRCRAFT_OBJ_PATH);
    smallAircraft1.scale *= 0.5;
    smallAircraft1.translation = glm::vec3(100.0f, -10.0f, -10.0f);
    mediumAircraft1.children.push_back(&smallAircraft1);

    auto smallAircraft2 = scene::node_t{};
    smallAircraft2.model = model::load(AIRCRAFT_OBJ_PATH);
    smallAircraft2.scale *= 0.5;
    smallAircraft2.translation = glm::vec3(-100.0f, -10.0f, -10.0f);
    mediumAircraft2.children.push_back(&smallAircraft2);

    // load the car
    auto car = scene::node_t{};
    car.model = model::load(CAR_PATH);
    car.translation = glm::vec3(3.0f, 0.0f, 0);
    track.children.push_back(&car);

    // load the root scene
    auto scene = scene::scene_t{euler_camera::make_camera(glm::vec3(0, 3, 7), glm::vec3(0)), &track};
    auto renderer = renderer::init(
        glm::perspective(glm::radians(60.0), (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.1, 1000.0));

    // load the point light
    auto light_cube = scene::node_t{};
    light_cube.model.meshes.push_back(mesh::init(shapes::make_cube(0.1f)));
    light_cube.model.materials.push_back({.ambient = glm::vec3(100)});
    light_cube.translation = renderer.spot_light_pos[0];
    track.children.push_back(&light_cube);

    // load the point light
    auto light_cube1 = scene::node_t{};
    light_cube1.model.meshes.push_back(mesh::init(shapes::make_cube(0.1f)));
    light_cube1.model.materials.push_back({.ambient = glm::vec3(1.0, 0, 1.0)});
    light_cube1.translation = renderer.spot_light_pos[1];
    track.children.push_back(&light_cube1);

    // TODO - turn this on or off?
    // glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(win))
    {
        float dt = (float)time_delta();
        auto now = (float)glfwGetTime();

        euler_camera::update_camera(scene.camera, win, dt);

        update_light_cube(win, &light_cube, dt);
        update_light_cube(win, &light_cube1, dt);

        if (glfwGetKey(win, GLFW_KEY_U) == GLFW_PRESS)
        {
            specialView = true;
        }

        if (specialView)
        {
            special_view(win, scene.camera, largeAircraft);
        }

        renderer.spot_light_pos[0] = light_cube.translation;
        renderer.spot_light_pos[1] = light_cube1.translation;

        car.rotation.y -= 2.0 * dt;

        // change the position of the aircraft
        // the control point for bazier curve
        std::vector<glm::vec3> s1 = {
            {0, 0.3, 0},
            {-0.3, 0.3, 0.25},
            {-0.25, 0.3, -0.55},
            {0, 0.3, -0.75},
        };

        std::vector<glm::vec3> s2 = {
            {0, 0.3, 0},
            {0.3, 0.3, 0.25},
            {0.25, 0.3, -0.55},
            {0, 0.3, -0.75},
        };

        float scale = 37.0f;
        for (glm::vec3 &p : s1)
        {
            p *= scale;
        }

        for (glm::vec3 &p : s2)
        {
            p *= scale;
        }

        float t = fmod(0.25 * now, 2.0f) - 1.0f;
        glm::vec3 pos = t > 0.0f ? cubic_bezier(s1, t) : cubic_bezier(s2, -t);
        largeAircraft.translation = pos;

        renderer::render(renderer, scene);

        glfwSwapBuffers(win);
        glfwPollEvents();

        // not entirely correct as a frame limiter, but close enough
        // it would be more correct if we knew how much time this frame took to render
        // and calculated the distance to the next "ideal" time to render and only slept that long
        // the current way just always sleeps for 16.67ms, so in theory we'd drop frames
        // std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000.f / 60));
    }

    // deleting the whole window also removes the opengl context, freeing all our memory in one fell swoop.
    chicken3421::delete_opengl_window(win);

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
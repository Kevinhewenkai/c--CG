#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ass3/cubemap_camera.hpp>

namespace cubemap_camera
{

    cubemap_camera_t make_camera(glm::vec3 pos, glm::mat4 proj)
    {
        cubemap_camera_t camera;
        camera.pos = pos;
        camera.proj = proj;
        return camera;
    }

    glm::mat4 get_view(const cubemap_camera_t &cam)
    {
        // calculate the rotated coordinate frame
        return cam.view;
    }

    void update_view(cubemap_camera_t &cam, glm::mat4 view)
    {
        cam.view = view;
    }

} // namespace euler_camera

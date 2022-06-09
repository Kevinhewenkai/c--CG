#ifndef COMP3421_CUBEMAP_CAMERA_HPP
#define COMP3421_CUBEMAP_CAMERA_HPP

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace cubemap_camera
{

    struct cubemap_camera_t
    {
        glm::vec3 pos;
        glm::mat4 proj;
        glm::mat4 view;
    };

    /**
	 *
	 * @param pos - position of the camera
	 * @param target - point in word space the camera is looking at
	 * @return initialised camera_t struct
	 */
    cubemap_camera_t make_camera(glm::vec3 pos, glm::mat4 proj);

    /**
	 * Get the view matrix of the camera (aka inverse transformation of cam)
	 * @param cam - camera data
	 * @return - view matrix
	 */
    glm::mat4 get_view(const cubemap_camera_t &cam);

    /**
	 * Update the camera's position on wasd keys as well as the direction the camera's facing based
	 * on cursor movement
	 * @param cam - cam info
	 * @param window - glfw window
	 * @param dt - delta time
	 */
    void update_view(cubemap_camera_t &cam, glm::mat4 view);

} // namespace euler_camera

#endif // COMP3421_EULER_CAMERA_HPP

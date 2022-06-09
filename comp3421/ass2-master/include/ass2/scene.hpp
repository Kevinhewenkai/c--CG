#ifndef COMP3421_ASS2_SCENE_HPP
#define COMP3421_ASS2_SCENE_HPP

#include "model.hpp"
#include "euler_camera.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

namespace scene
{
	struct node_t
	{
		model::model_t model;
		glm::vec3 translation = glm::vec3(0.0);
		glm::vec3 rotation = glm::vec3(0.0); // vec3 of euler angles
		glm::vec3 scale = glm::vec3(1.0);
		std::vector<node_t *> children;
	};

	struct scene_t
	{
		euler_camera::camera_t camera;
		node_t *root;
	};
} // namespace scene

#endif // COMP3421_SCENE_HPP

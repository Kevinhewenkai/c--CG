#include "ass3/animation.hpp"
#include "ass3/scene.hpp"

namespace animation
{
	void animate(scene::node_t &node, float dt)
	{
		switch (node.kind)
		{
		case scene::node_t::REFLECTIVE:
			node.rotation.y += dt;
			break;
		default:
			break;
		}
		for (auto &child : node.children)
		{
			animate(child, dt);
		}
	}
} // namespace animation

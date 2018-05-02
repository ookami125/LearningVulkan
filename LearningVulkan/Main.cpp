//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "VulkanRenderer.h"
#include "Model.h"

int main(int argc, char** argv)
{
	int width = 800;
	int height = 600;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

	VulkanRenderer* renderer = new VulkanRenderer();
	//renderer->Init(window, true);

	//Model* character = new Model("models/Samba Dancing.fbx");
	//Model* teapot = new Model("models/teapot.obj");

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	delete renderer;
	glfwDestroyWindow(window);
	return 0;
}
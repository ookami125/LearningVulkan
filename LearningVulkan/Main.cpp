//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "VulkanRenderer.h"

int main(int argc, char** argv)
{
	int width = 800;
	int height = 600;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

	VulkanRenderer* vulkan = new VulkanRenderer();
	if (!vulkan->Init(window, true))
	{
		printf("Error: VulkanRenderer failed to initalize!");
		system("pause");
		return 1;
	}

	delete vulkan;
	glfwDestroyWindow(window);
	system("pause");
	return 0;
}
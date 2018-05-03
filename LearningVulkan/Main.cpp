#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>

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
	HWND handle = glfwGetWin32Window(window);

	VulkanRenderer* renderer = new VulkanRenderer(handle);

	//Model* teapot = new Model("models/teapot.obj");

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		renderer->Render();
	}

	delete renderer;
	glfwDestroyWindow(window);
	return 0;
}
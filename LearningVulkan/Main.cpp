#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "VulkanRenderer.h"
#include "Model.h"
#include "VulkanModelData.h"
#include <algorithm>
#include <chrono>
#include <thread>

//bool stopping = false;
//void animationStuff(Model* model)
//{
//	while (!stopping)
//	{
//
//	}
//}

int main(int argc, char** argv)
{
	int width = 800;
	int height = 600;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	HWND handle = glfwGetWin32Window(window);

	VulkanRenderer* renderer = new VulkanRenderer(handle);

	Model* character = new Model("models/Box_Walk.fbx");
	Model* character2 = new Model("models/Samba Dancing.fbx");
	//character->LoadAnimations("models/Head Spinning.fbx");

	renderer->RegisterModel(character);
	renderer->RegisterModel(character2);

	//std::thread animationThread(animationStuff, character);

	//renderer->AllocShit(character->textures[0]);

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		renderer->StartRender();
		renderer->RenderModel(character);
		renderer->RenderModel(character2);
		renderer->EndRender();
		renderer->Present();
	}

	//stopping = true;
	//animationThread.join();

	renderer->UnregisterModel(character);
	delete character;
	renderer->UnregisterModel(character2);
	delete character2;

	delete renderer;
	glfwDestroyWindow(window);
	return 0;
}
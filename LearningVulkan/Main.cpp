#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>

#include "VulkanRenderer.h"
#include "Model.h"
#include "VulkanModelData.h"
#include <algorithm>
#include "Memory.h"
#include "Animation.h"
//#include <chrono>
//#include <thread>

int main(int argc, char** argv)
{
	int width = 800;
	int height = 600;

	//Bone* bone = aligned_new(Bone)(nullptr, 0, nullptr);
	//for (int i = 0; i < 1000; ++i)
	//{
	//	Vec4f* test = aligned_new(Vec4f)();
	//	if(((ptrdiff_t)test) % alignof(Vec4f))
	//		printf("alignment is %d: %d", alignof(Vec4f), ((ptrdiff_t)test) % alignof(Vec4f));
	//}


	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	HWND handle = glfwGetWin32Window(window);
	
	VulkanRenderer* renderer = new VulkanRenderer(handle);
	
	Model* character = aligned_new(Model)("models/Box_Walk.fbx");
	Model* character2 = aligned_new(Model)("models/Samba Dancing.fbx");
	
	renderer->RegisterModel(character);
	renderer->RegisterModel(character2);
	
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		renderer->StartRender();
		renderer->RenderModel(character);
		renderer->RenderModel(character2);
		renderer->EndRender();
		renderer->Present();
	}
	
	renderer->UnregisterModel(character);
	aligned_delete(character);
	renderer->UnregisterModel(character2);
	aligned_delete(character2);
	
	delete renderer;
	glfwDestroyWindow(window);
	return 0;
}
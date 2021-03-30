#include "GameInterface.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CubeFinal.h"

CubeFinal gCube;

GameInterface* gUsedInterface;

GLFWwindow* InitializeSystem() 
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Rubiks Cube", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	glewExperimental = GL_TRUE;
	glewInit();

	gUsedInterface->Initialize(window);

	return window;
}

void RunCoreloop(GLFWwindow* window)
{
	double lastTime = glfwGetTime();
	double timeDifference = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		gUsedInterface->Update(timeDifference);

		int screenWidth, screenHeight;
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		float aspectRatio = (float)screenWidth / (float)screenHeight;
		glViewport(0, 0, screenWidth, screenHeight);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gUsedInterface->Render(aspectRatio);
		glfwSwapBuffers(window);

		double currentTime = glfwGetTime();
		timeDifference = currentTime - lastTime;
		lastTime = currentTime;
	}
}

void ShutdownSystem()
{
	gUsedInterface->ClearResources();
	glfwTerminate();
}

int main()
{
	gUsedInterface = &gCube;

	GLFWwindow* window = InitializeSystem();
	RunCoreloop(window);
	ShutdownSystem();
}


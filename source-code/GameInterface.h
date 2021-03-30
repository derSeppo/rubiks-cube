#pragma once
class GLFWwindow;

class GameInterface
{
public:
	virtual void Initialize() {};
	virtual void Initialize(GLFWwindow* window) { Initialize(); }

	virtual void Update(double deltaTime) {};
	virtual void Render(float aspectRatio) {};
			
	virtual void ClearResources() {};
};


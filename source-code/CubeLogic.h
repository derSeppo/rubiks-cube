#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class CubeLogic
{
public:
	void Initialize();
	void RandomizeRotation(int rotations = 100);
	void ResetRotation();
	void RotateCubelets(glm::vec3 axis, int disc, int direction);
	glm::mat4 GetCubeletRotation(int x, int y, int z);
private:
	const glm::vec4 m_cubeCenterOffset = glm::vec4(1.0f);

	glm::mat4 m_cubeletRotations[2][3][3][3];

	glm::mat4 RoundMatrix(glm::mat4 m);

};


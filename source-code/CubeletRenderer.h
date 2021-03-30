#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include<vector>

class CubeletRenderer
{
public:
	void Initialize();

	void Render(const glm::mat4& transformationMatrix);

	void ClearResources();

	float GetCubeletExtension() const { return 2.0f * m_offset; };

private:

	const float m_offset = 0.5f;

	void AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positionArray);
	void AddSideColor(int sideType, int direction, std::vector < glm::vec3>& colorArray);
	void TranscribeToFloatArray(std::vector<glm::vec3>& vecArray, float* floatArray);

	GLuint m_arrayBufferObject;
	GLuint m_vertexBufferObject[2];
	GLuint m_shaderProgram;
	GLint m_transformLocation;
};


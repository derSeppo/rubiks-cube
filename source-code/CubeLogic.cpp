#include "CubeLogic.h"
#include <iostream>

#include <Windows.h>

void CubeLogic::Initialize()
{
	srand(time(NULL));

	ResetRotation();
}

void CubeLogic::RandomizeRotation(int rotations)
{
	for (int i = 0; i < rotations; i++)
	{
		int axisIndex = rand() % 3;
		int disc = rand() % 3;
		int direction = rand() % 2 * 2 - 1;

		glm::vec3 axis = glm::vec3(0.0f);
		axis[axisIndex] = 1.0f;

		RotateCubelets(axis, disc, direction);
	}
}

void CubeLogic::ResetRotation()
{
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			for (int z = 0; z < 3; z++)
			{
				m_cubeletRotations[0][x][y][z] = glm::mat4(1.0f);
				m_cubeletRotations[1][x][y][z] = glm::mat4(1.0f);
			}
		}
	}
}


void CubeLogic::RotateCubelets(glm::vec3 axis, int disc, int direction)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f*direction), axis);

	for(int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			for (int z = 0; z < 3; z++)
			{
				glm::vec4 coords = glm::vec4(x,y,z,1.0f);

				//Cubelet is to be rotated, if the coordiantes projected on the rotation axis equal the disc value
				if (glm::dot((glm::vec3)coords, axis) == disc)
				{
					//Translate the coordinates to the origin, rotate and translate back
					glm::vec4 rotatedCoords = rotation * (coords - m_cubeCenterOffset) + m_cubeCenterOffset;

					//Apply the rotation to the stored values and write them to the rotated Coordinates
					m_cubeletRotations[1][(int)round(rotatedCoords.x)][(int)round(rotatedCoords.y)][(int)round(rotatedCoords.z)] = rotation * m_cubeletRotations[0][x][y][z];
				}
			}
		}
	}

	//Equalize the final and temporary rotation matrices
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				
				//Round rotation values to prevent presicion errors
				m_cubeletRotations[1][x][y][z] = RoundMatrix(m_cubeletRotations[1][x][y][z]);

				m_cubeletRotations[0][x][y][z] = m_cubeletRotations[1][x][y][z];
			}
		}
	}
}

glm::mat4 CubeLogic::GetCubeletRotation(int x, int y, int z)
{
	return m_cubeletRotations[0][x][y][z];
}

glm::mat4 CubeLogic::RoundMatrix(glm::mat4 m)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = round(m[i][j]);
		}
	}

	return m;
}

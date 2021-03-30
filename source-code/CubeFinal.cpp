#include "CubeFinal.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <Windows.h>

void CubeFinal::Initialize(GLFWwindow* window)
{
	m_input.SetWindow(window);
	m_input.ObserveKey(GLFW_KEY_SPACE);

	m_input.ObserveKey(GLFW_KEY_W);
	m_input.ObserveKey(GLFW_KEY_A);
	m_input.ObserveKey(GLFW_KEY_S);
	m_input.ObserveKey(GLFW_KEY_D);

	m_input.ObserveKey(GLFW_KEY_Q);
	m_input.ObserveKey(GLFW_KEY_E);

	m_input.ObserveKey(GLFW_KEY_R);
	m_input.ObserveKey(GLFW_KEY_F);

	m_orientationQuaternion = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	m_isRotatingCube = false;

	m_dragState = DragState::NO_DRAG;
	ResetDragParams();

	m_cubeletRenderer.Initialize();

	m_cubeLogic.Initialize();
}

void CubeFinal::Update(double deltaTime)
{
	m_input.Update();

	//Shuffle the Cube
	if (m_input.WasKeyPressed(GLFW_KEY_R))
		m_cubeLogic.RandomizeRotation();

	//Finish/Reset the Cube
	if (m_input.WasKeyPressed(GLFW_KEY_F))
		m_cubeLogic.ResetRotation();

	UpdateDrag(deltaTime);

	UpdateRotation(deltaTime);
}

void CubeFinal::Render(float aspectRatio)
{
	m_viewProjection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, -9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::mat4_cast(m_orientationQuaternion);

	float offset = m_cubeletRenderer.GetCubeletExtension();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				glm::mat4 compoundTransformation = m_viewProjection;

				if ((m_dragState == DragState::DRAG_PARAMS_SET || m_dragState == DragState::DRAG_COMPLETING)
					&& glm::dot(glm::vec3(i, j, k), m_rotationAxis) == m_dragDisc)
				{
					glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_dragAmount * 90.0f), m_rotationAxis);

					compoundTransformation *= rotation;
				}

				compoundTransformation = glm::translate(compoundTransformation, glm::vec3((i - 1) * offset, (j - 1) * offset, (k - 1) * offset));

				compoundTransformation *= m_cubeLogic.GetCubeletRotation(i, j, k);

				m_cubeletRenderer.Render(compoundTransformation);
			}
		}
	}
}

void CubeFinal::ClearResources()
{
	m_cubeletRenderer.ClearResources();
}

void CubeFinal::UpdateRotation(double deltaTime)
{
	//Reset rotation
	if (m_input.WasKeyPressed(GLFW_KEY_SPACE))
		m_orientationQuaternion = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	//Keyboard rotation around the x axis
	float xVel = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_W))
		xVel = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_S))
		xVel = glm::radians(-90.0f);

	//Keyboard rotation around the y axis
	float yVel = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_D))
		yVel = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_A))
		yVel = glm::radians(-90.0f);

	//Keyboard rotation around the z axis
	float zVel = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_E))
		zVel = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_Q))
		zVel = glm::radians(-90.0f);

	glm::quat rotationQuaternion = glm::quat(0.0f, glm::vec3(xVel, yVel, zVel));

	m_orientationQuaternion += 0.5f * (float)deltaTime * rotationQuaternion * m_orientationQuaternion;
	m_orientationQuaternion = normalize(m_orientationQuaternion);

	//Mouse rotation
	//Difference between prior and current mouse position as the factor of rotation
	//Vertical rotation inverted, because the x axis is flipped from the cameras point of view
	if (m_input.IsRightMouseDown()) {
		if (!m_isRotatingCube) {
			m_isRotatingCube = true;

			m_input.GetMousePosition(m_rotationCubePrior.x, m_rotationCubePrior.y);
		}

		glm::dvec2 dragCurrent;
		m_input.GetMousePosition(dragCurrent.x, dragCurrent.y);

		float vRot = -(dragCurrent.y - m_rotationCubePrior.y) / 100.0f * m_mouseRotationSpeed;
		float hRot = (dragCurrent.x - m_rotationCubePrior.x) / 100.0f * m_mouseRotationSpeed;

		vRot = glm::radians(vRot);
		hRot = glm::radians(hRot);

		m_rotationCubePrior = dragCurrent;

		glm::quat rotationQuaternion = glm::quat(0.0f, glm::vec3(vRot, hRot, 0.0f));

		m_orientationQuaternion += (float)deltaTime * rotationQuaternion * m_orientationQuaternion;
		m_orientationQuaternion = normalize(m_orientationQuaternion);
	}
	else if (m_isRotatingCube) {
		m_isRotatingCube = false;
	}
}

void CubeFinal::UpdateDrag(double deltaTime)
{
	if (m_input.IsLeftMouseDown()) {

		glm::vec3 position, direction;

		m_input.GetPickingRay(m_viewProjection, position, direction);

		if (m_dragState == DragState::NO_DRAG) {

			if (FindIntersectingFace(position, direction))
			{
				m_dragCubeletPrior = position;

				m_dragState = DragState::DRAG_PARAMS_NOT_SET;
			}
		}
		else if (m_dragState != DragState::DRAG_COMPLETING)
		{
			m_dragAmountVector += position - m_dragCubeletPrior;

			m_dragCubeletPrior = position;

			glm::vec2 z = m_viewProjection * glm::vec4(m_dragAmountVector, 1.0f);

			if (m_dragState == DragState::DRAG_PARAMS_NOT_SET) {
				if (glm::dot(z, z) > (m_minDrag * m_minDrag)) {
					glm::vec2 h = m_viewProjection * glm::vec4(m_intersectPlane[0], 1.0f);
					glm::vec2 k = m_viewProjection * glm::vec4(m_intersectPlane[1], 1.0f);

					h = glm::normalize(h);
					k = glm::normalize(k);

					if (abs(glm::dot(h, z)) < abs(glm::dot(k, z))) {
						m_rotationAxis = m_intersectPlane[0];
						m_dragDisc = m_intersectCoords[0];

						m_dragAxis = glm::vec2(h.y, -h.x);
					}
					else
					{
						m_rotationAxis = m_intersectPlane[1];
						m_dragDisc = m_intersectCoords[1];

						m_dragAxis = glm::vec2(k.y, -k.x);
					}

					if (m_dragDisc < -0.5)
						m_dragDisc = 0;
					else if (m_dragDisc < 0.5)
						m_dragDisc = 1;
					else
						m_dragDisc = 2;

					m_dragState = DragState::DRAG_PARAMS_SET;
				}
			}
			else
			{
				m_dragAmount = glm::dot(m_dragAxis, z);
				m_dragAmount *= m_mouseDragSpeed;
			}
		}
	}
	else
	{
		if (m_dragState == DragState::DRAG_PARAMS_SET)
		{
			m_dragState = DragState::DRAG_COMPLETING;
			m_lerpStartRotationAmount = m_dragAmount;
		}
		else if (m_dragState == DragState::DRAG_PARAMS_NOT_SET)
		{
			ResetDragParams();
		}
	}

	if (m_dragState == DragState::DRAG_COMPLETING)
	{
		float targetRotationAmount = round(m_lerpStartRotationAmount);
		float lerpDifference = targetRotationAmount - m_dragAmount;

		float lerpTimeDelta = (float)deltaTime * m_dragLerpSpeed;

		if (m_lerpTime <= 1.0f && (lerpDifference > m_dragLerpCutoff || lerpDifference < -m_dragLerpCutoff))
		{
			m_lerpTime += lerpTimeDelta;
			m_lerpTime = glm::clamp(m_lerpTime, 0.0f, 1.0f);

			m_dragAmount = m_lerpStartRotationAmount * (1.0f - m_lerpTime) + targetRotationAmount * m_lerpTime;
		}
		else
		{
			int direction = m_dragAmount > 0 ? 1 : -1;

			for (int i = 0; i < abs(targetRotationAmount); i++)
				m_cubeLogic.RotateCubelets(m_rotationAxis, m_dragDisc, direction);

			ResetDragParams();

			PlaySound(TEXT("Click.wav"), NULL, SND_FILENAME);
		}
	}
}

void CubeFinal::ResetDragParams()
{
	m_rotationAxis = glm::vec3(0.0f);
	m_dragAxis = glm::vec2(0.0f);
	m_dragDisc = 0;

	m_dragAmountVector = glm::vec3(0.0f);
	m_dragAmount = 0.0f;

	m_lerpTime = 0.0f;

	m_dragState = DragState::NO_DRAG;
}

bool CubeFinal::FindIntersectingFace(glm::vec3 position, glm::vec3 direction)
{
	for (int i = 0; i < 6; i++)
	{
		glm::vec3 normal = glm::vec3(0.0f);
		normal[i % 3] = pow(-1.0f, i);

		if (glm::dot(direction, normal) < 0)
		{
			glm::vec3 intersect = position + direction * ((1.5f - glm::dot(position, normal)) / glm::dot(direction, normal));

			m_intersectPlane[0] = glm::vec3(0.0f);
			m_intersectPlane[0][(i + 1) % 3] = 1.0f;

			m_intersectPlane[1] = glm::vec3(0.0f);
			m_intersectPlane[1][(i + 2) % 3] = 1.0f;

			//Schnittpunkt auf x-Achse (der Ebene) projiziert
			float x = glm::dot(m_intersectPlane[0], intersect - 1.5f * normal);

			//Schnittpunkt auf y-Achse (der Ebene) projiziert
			float y = glm::dot(m_intersectPlane[1], intersect - 1.5f * normal);

			if ((x > -1.5001 && x < 1.5001) && (y > -1.5001 && y < 1.5001)) {
				m_intersectCoords[0] = x;
				m_intersectCoords[1] = y;

				return true;
			}
		}
	}
	return false;
}

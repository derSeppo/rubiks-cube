#pragma once

#include "GameInterface.h"
#include "CubeletRenderer.h"
#include "CubeLogic.h"
#include "InputSystem.h"
#include <glm/ext/quaternion_float.hpp>

class CubeFinal : public GameInterface
{
	public:
		virtual void Initialize(GLFWwindow* window);
	
		virtual void Update(double deltaTime);
		virtual void Render(float aspectRatio);
	
		virtual void ClearResources();
	
	private:
		enum class DragState {
			NO_DRAG,
			DRAG_PARAMS_NOT_SET,
			DRAG_PARAMS_SET,
			DRAG_COMPLETING,
		};

		const float m_mouseDragSpeed = 5.0f;
		const float m_minDrag = 0.01f;

		const float m_dragLerpSpeed = 2.0f;
		const float m_dragLerpCutoff = 0.001f;
	
		const float m_mouseRotationSpeed = 600.0f;
	
		CubeletRenderer m_cubeletRenderer;
		CubeLogic m_cubeLogic;
		InputSystem m_input;

		glm::mat4 m_viewProjection;
		glm::quat m_orientationQuaternion;
	
		bool m_isRotatingCube;
		glm::dvec2 m_rotationCubePrior;
	
		DragState m_dragState;
		float m_intersectCoords[2];
		glm::vec3 m_intersectPlane[2];

		glm::vec3 m_dragCubeletPrior;
		glm::vec3 m_dragAmountVector;
		float m_dragAmount;
	
		glm::vec3 m_rotationAxis;
		glm::vec2 m_dragAxis;

		float m_dragDisc;
	
		float m_lerpTime;
		float m_lerpStartRotationAmount;
	
		void UpdateRotation(double deltaTime);

		void UpdateDrag(double deltaTime);
		void ResetDragParams();

		bool FindIntersectingFace(glm::vec3 position, glm::vec3 direction);
};


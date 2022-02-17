#pragma once

#include "3d_shapes.h"

enum class cameraDirection { front, right, up };

class Camera {
public:
	GLfloat scale;

	glm::vec3 position;
	glm::vec3 eulerRotation;
	glm::vec3 target_vec;

	glm::mat4 view_mat;
	glm::mat4 projection_mat;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) {
		view_mat = glm::mat4(1.0f);
		projection_mat = glm::mat4(1.0f);

		this->position = position;
		target_vec = glm::vec3(0.0f, 0.0f, 0.0f);
		eulerRotation = glm::vec3(0.0f, 0.0f, 0.0f);

		scale = 1.0f;

		view_mat = glm::translate(view_mat, position);
	}

	// since this a viewport camera, the camera rotates around an origin (target_vec)
	// we do not have to translate the view matrix for every translation call, just translating the target does the work
	glm::mat4 GetViewMat() {
		return glm::translate(view_mat, target_vec);
	}

	void Translate(glm::vec3 offset) {
		target_vec -= offset;
	}

	void TranslateLocal(GLfloat offset, cameraDirection dir) {
		offset *= -1;

		GLfloat rotZ = eulerRotation.z;
		glm::vec3 direction(0.0f, 0.0f, 0.0f);

		if (dir == cameraDirection::front)
			direction = glm::vec3(glm::sin(glm::radians(rotZ)), glm::cos(glm::radians(rotZ)), 0.0f);
		else if(dir == cameraDirection::right)
			direction = glm::vec3(glm::sin(glm::radians(rotZ + 90.0f)), glm::cos(glm::radians(rotZ + 90.0f)), 0.0f);

		target_vec += offset * direction;
	}

	void Rotate(GLfloat eulerAngle, glm::vec3 direction) {
		view_mat = glm::rotate(view_mat, glm::radians(eulerAngle), direction);
		
		eulerRotation += direction * eulerAngle;

		for (GLuint i = 0; i < 3; i++) {
			if (eulerRotation[i] >= 360.0f) {
				eulerRotation[i] -= 360.0f;
			}
			else if (eulerRotation[i] <= -360.0f) {
				eulerRotation[i] += 360.0f;
			}
		}
	}

	void RotateLocalX(GLfloat eulerAngle) {
		GLfloat rotZ = eulerRotation.z;

		Rotate(-rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
		Rotate(eulerAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		Rotate(rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void Scale(GLfloat scale) {
		this->scale *= scale;

		view_mat = glm::scale(view_mat, scale * glm::vec3(1.0f, 1.0f, 1.0f));
	}
};
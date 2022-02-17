#pragma once

#include "3d_shapes.h"
#include "camera.hpp"

class Empty {
protected:
	GLuint VAO;
	GLuint VBO;

	void BindVertices() {
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * vertCount, vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

public:
	glm::vec3 position;
	glm::vec3 rotation;

	GLfloat* vertices;
	GLfloat lineWidth;

	GLuint vertCount;
	GLuint shaderProgram;

	Empty(GLuint vertCount, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), GLfloat lineWidth = 1.0f) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		this->vertCount = vertCount;
		this->vertices  = new GLfloat[vertCount * 3];

		this->position	= position;
		this->rotation	= glm::vec3(0.0f, 0.0f, 0.0f);

		this->lineWidth = lineWidth;

		this->shaderProgram = 0;
	}

	void SetShader(GLuint shader, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {
		shaderProgram = shader;

		glUseProgram(shaderProgram);

		GLuint color_loc = glGetUniformLocation(shaderProgram, "vertColor");

		if (color_loc != -1) glUniform4f(color_loc, color[0], color[1], color[2], color[3]);

		glUseProgram(0);
	}

	void SetMats(Camera camera) {
		glUseProgram(shaderProgram);

		GLuint proj_loc = glGetUniformLocation(shaderProgram, "projection");
		GLuint view_loc = glGetUniformLocation(shaderProgram, "view");
		GLuint scale_loc = glGetUniformLocation(shaderProgram, "worldScale");

		if (proj_loc != -1) glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(camera.projection_mat));
		if (view_loc != -1) glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMat()));
		if (scale_loc != -1) glUniform1f(scale_loc, camera.scale);

		glUseProgram(0);
	}

	virtual void Draw(Camera camera) {
		SetMats(camera);

		glLineWidth(lineWidth);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(VAO);
		glUseProgram(shaderProgram);

		glDrawArrays(GL_LINES, 0, vertCount);

		glUseProgram(0);
		glBindVertexArray(0);
		glLineWidth(1.0f);
	}

	~Empty() {
		delete[] vertices;

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
};

class Line : public Empty {
public:
	Line(glm::vec3 start, glm::vec3 end, GLfloat lineWidth = 1.0f) : Empty(2, 0.5f * (start + end), lineWidth) {
		vertices[0] = start.x;
		vertices[1] = start.y;
		vertices[2] = start.z;

		vertices[3] = end.x;
		vertices[4] = end.y;
		vertices[5] = end.z;

		BindVertices();
	}
};

class Grid : public Empty {
public:
	Grid(GLfloat offset = 1.0f, GLuint count = 10, GLfloat lineWidht = 1.0f, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) : Empty(count * 8, position, lineWidth) {
		GLuint index = 0;
		GLfloat bound = count;

		for (GLuint i = 0; i < count - 1; i++) {
			for (GLint j = -1; j <= 1; j += 2) {
				vertices[index++] = -bound;
				vertices[index++] = j * offset * (i + 1);
				vertices[index++] = 0.0f;

				vertices[index++] = bound;
				vertices[index++] = j * offset * (i + 1);
				vertices[index++] = 0.0f;

				vertices[index++] = j * offset * (i + 1);
				vertices[index++] = -bound;
				vertices[index++] = 0.0f;

				vertices[index++] = j * offset * (i + 1);
				vertices[index++] = bound;
				vertices[index++] = 0.0f;
			}
		}

		BindVertices();
	}
};
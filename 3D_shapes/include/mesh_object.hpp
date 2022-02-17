#pragma once

#include "3d_shapes.h"
#include "camera.hpp"

class MeshObject {
protected:
	GLuint VBO;
	GLuint EBO;
	GLuint VAO;

	virtual void BindBuffers(GLboolean elementBuffer = GL_TRUE) {
		glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

			glBufferData(GL_ARRAY_BUFFER, attribCount * this->vertCount * sizeof(GLfloat), this->vertices, GL_STATIC_DRAW);

			if (elementBuffer) {
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * this->triCount * sizeof(GLuint), this->indices, GL_STATIC_DRAW);
			}

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attribCount * sizeof(GLfloat), (void*)0);
			if (attribCount == 6) {
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, attribCount * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
			}
			glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

public:
	GLuint vertCount;
	GLuint triCount;

	GLfloat* vertices;
	GLuint*  indices;

	GLuint shaderProgram;
	GLuint attribCount;

	glm::vec3 position;
	glm::mat4 model_mat;

	MeshObject(GLuint vertCount, GLuint triCount, GLuint attribs = 3) {
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);

		position = glm::vec3(0.0f, 0.0f, 0.0f);

		this->vertCount = vertCount;
		this->triCount  = triCount;
		this->attribCount = attribs;

		this->vertices = new GLfloat[attribs * this->vertCount];
		this->indices  = new GLuint[3 * this->triCount];

		this->shaderProgram = 0;
		this->model_mat = glm::mat4(1.0f);
	}

	void SetMats(Camera camera) {
		glUseProgram(shaderProgram);

		GLuint proj_loc = glGetUniformLocation(shaderProgram, "projection");
		GLuint view_loc = glGetUniformLocation(shaderProgram, "view");
		GLuint norm_loc = glGetUniformLocation(shaderProgram, "normal_mat");

		if (proj_loc != -1) glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(camera.projection_mat));
		if (view_loc != -1) glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMat()));
		if (norm_loc != -1) glUniformMatrix3fv(norm_loc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(camera.GetViewMat())))));

		glUseProgram(0);
	}

	virtual void Draw(Camera camera, GLenum polygonMode = GL_FILL, GLenum drawMode = GL_TRIANGLES) {
		SetMats(camera);

		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

		glBindVertexArray(this->VAO);
		glUseProgram(this->shaderProgram);

		glDrawElements(drawMode, 3 * this->triCount, GL_UNSIGNED_INT, 0);

		glUseProgram(0);
		glBindVertexArray(0);
	}

	void SetShader(GLuint program) {
		this->shaderProgram = program;
	}

	void Rotate(glm::vec3 axis, GLfloat angle) {
		model_mat = glm::rotate(model_mat, angle, axis);
	}
	
	~MeshObject() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);


		delete[] this->vertices;
		delete[] this->indices;
	}
};

class Disk: public MeshObject {
private:
	void GenerateVertices() {
		GLfloat offset = (2 * PI) / (this->vertCount - 1);

		// add a center vert for triangulation, terrible way to triangulate for now
		// clean this up later
		this->vertices[0] = this->position.x;
		this->vertices[1] = this->position.y;
		this->vertices[2] = this->position.z;

		GLfloat theta = 0;

		// add the polygon vertices to the array
		for (GLuint i = 0; i < this->triCount; i++) {
			this->vertices[(i + 1) * 3]		= this->position.x + this->radius * glm::cos(theta);
			this->vertices[(i + 1) * 3 + 1] = this->position.y + this->radius * glm::sin(theta);
			this->vertices[(i + 1) * 3 + 2] = this->position.z;

			theta += offset;
		}

		// fill the indices
		for (GLuint i = 0; i < this->triCount; i++) {
			this->indices[i * 3] = 0;
			this->indices[i * 3 + 1] = i + 1;
			this->indices[i * 3 + 2] = ((i + 2) == (this->triCount + 1)) ? 1 : (i + 2);
		}
	}

public:
	GLfloat   radius;
	GLuint    resolution;

	Disk(GLfloat radius = 1.0f, GLuint resolution = 16, glm::vec3 cPos = glm::vec3(0.0f, 0.0f, 0.0f)):MeshObject(resolution + 1, resolution) {
		this->radius = radius;
		this->position = cPos;
		this->resolution = resolution;

		GenerateVertices();

		BindBuffers();
	}
};

class UVSphere : public MeshObject {
private:
	void GenerateVertices() {
		GLfloat offsetY = 180.0f / (divisionsY + 1);
		GLfloat offsetX = 360.0f / divisionsX;

		// after the first point, start from the first circle
		GLfloat angleY = offsetY;
		GLuint index = 0;

		// south pole
		vertices[index++] = 0.0f + position.x;
		vertices[index++] = 0.0f + position.y;
		vertices[index++] = position.z - radius;

		// normal
		vertices[index++] = 0.0f;
		vertices[index++] = 0.0f;
		vertices[index++] = -1.0f;

		for (GLuint i = 0; i < divisionsY; i++) {
			// calculate the radius and z location for each level
			GLfloat phi	= 0.0f;
			GLfloat l_rad = radius * glm::cos(glm::radians(angleY - 90.0f));
			GLfloat z_pos = radius * glm::sin(glm::radians(angleY - 90.0f));

			// generate vertices for a circle
			for (GLuint j = 0; j < divisionsX; j++) {
				glm::vec3 point(
					position.x + l_rad * glm::cos(glm::radians(phi)),
					position.y + l_rad * glm::sin(glm::radians(phi)),
					position.z + z_pos
				);
				vertices[index++] = point.x;
				vertices[index++] = point.y;
				vertices[index++] = point.z;
				
				// calculate normals
				glm::vec3 normal = glm::normalize(point - position);
				vertices[index++] = normal.x;
				vertices[index++] = normal.y;
				vertices[index++] = normal.z;

				phi += offsetX;
			}

			angleY += offsetY;
		}

		// north pole
		vertices[index++] = 0.0f + position.x;
		vertices[index++] = 0.0f + position.y;
		vertices[index++] = position.z + radius;

		// normal
		vertices[index++] = 0.0f;
		vertices[index++] = 0.0f;
		vertices[index++] = 1.0f;

		// 
		// generate indices;
		// 

		index = 0;

		// south pole
		for (GLuint i = 0; i < divisionsX; i++) {
			indices[index++] = 0;
			indices[index++] = i + 1;
			indices[index++] = (i + 2 == divisionsX + 1)? 1: (i + 2);
		}

		for (GLuint i = 0; i < divisionsY - 1; i++) {
			for (GLuint j = 0; j < divisionsX; j++) {
				indices[index++] = 1 + divisionsX * i + j;
				indices[index++] = 1 + divisionsX * i + ((j + 1 == divisionsX) ? 0 : (j + 1));
				indices[index++] = 1 + divisionsX * (i + 1) + ((j + 1 == divisionsX) ? 0 : (j + 1));

				indices[index++] = 1 + divisionsX * i + j;
				indices[index++] = 1 + divisionsX * (i + 1) + j;
				indices[index++] = 1 + divisionsX * (i + 1) + ((j + 1 == divisionsX) ? 0 : (j + 1));
			}
		}

		GLuint lastIndex = divisionsX * (divisionsY - 1) + 1;
		// north pole
		for (GLuint i = 0; i < divisionsX; i++) {
			indices[index++] = lastIndex + i;
			indices[index++] = lastIndex + ((i + 1 == divisionsX) ? 0 : i + 1);
			indices[index++] = lastIndex + divisionsX;
		}
	}

public:
	GLfloat	  radius;
	GLuint	  divisionsX;
	GLuint	  divisionsY;

	UVSphere(GLfloat radius = 1.0f, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), GLuint divX = 16, GLuint divY = 16): MeshObject(
		divX * divY + 2,					// total vertices
		(divX * 2) * (divY - 1) + divX * 2, // total triangles
		6
	) {
		this->position	 = position;
		this->radius	 = radius;
		this->divisionsX = divX;
		this->divisionsY = divY;

		GenerateVertices();

		BindBuffers();
	}
};

class Torus : public MeshObject {
private:
	void GenerateVertices() {
		GLfloat offsetR = 360.0f / divisionsR;
		GLfloat offsetT = 360.0f / divisionsT;

		GLuint index = 0;
		GLfloat phi = 0.0f;

		for (GLuint i = 0; i < divisionsT; i++) {
			GLfloat theta = 0.0f;
			for (GLuint j = 0; j < divisionsR; j++) {
				glm::vec3 point(
					outerRadius + innerRadius * glm::cos(glm::radians(theta)),
					0,
					innerRadius * glm::sin(glm::radians(theta))
				);

				glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(0.0f, 0.0f, 1.0f));
				point = glm::vec3(rot * glm::vec4(point, 1.0f)) + position;

				vertices[index++] = point.x;
				vertices[index++] = point.y;
				vertices[index++] = point.z;

				theta += offsetR;

				// calculate normals
				glm::vec3 relPos = outerRadius * glm::vec3(glm::cos(glm::radians(phi)), glm::sin(glm::radians(phi)), 0.0f);
				glm::vec3 normal = glm::normalize(point - (position + relPos));

				vertices[index++] = normal.x;
				vertices[index++] = normal.y;
				vertices[index++] = normal.z;
			}

			phi += offsetT;
		}

		index = 0;

		// 
		// generate indices 
		//

		for (GLuint i = 0; i < divisionsT; i++) {
			for (GLuint j = 0; j < divisionsR; j++) {
				indices[index++] = i * divisionsR + j;
				indices[index++] = i * divisionsR + ((j + 1 == divisionsR) ? 0 : (j + 1));
				indices[index++] = ((i + 1 == divisionsT) ? 0 : (i + 1)) * divisionsR + ((j + 1 == divisionsR) ? 0 : (j + 1));

				indices[index++] = i * divisionsR + j;
				indices[index++] = ((i + 1 == divisionsT) ? 0 : (i + 1)) * divisionsR + j;
				indices[index++] = ((i + 1 == divisionsT) ? 0 : (i + 1)) * divisionsR + ((j + 1 == divisionsR) ? 0 : (j + 1));
			}
		}
	}

public:
	GLfloat innerRadius, outerRadius;
	GLuint divisionsR, divisionsT;

	Torus(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), GLfloat innerR = 0.5f, GLfloat outerR = 1.0f, GLuint divR = 8, GLuint divT = 32): MeshObject(
		divR * divT,
		(divR * 2) * divT,
		6
	) {
		this->position	  = position;
		this->innerRadius = innerR;
		this->outerRadius = outerR;
		this->divisionsR  = divR;
		this->divisionsT  = divT;

		GenerateVertices();

		BindBuffers();
	}
};

class Trefoil : public MeshObject {
private:
	void GenerateVertices() {
		GLfloat offsetL = 360.0f / divisionsL;
		GLfloat offsetN = 360.0f / divisionsN;

		GLfloat theta = 0.0;
		GLuint index = 0;

		std::vector <glm::vec3> points;

		for (GLuint i = 0; i < divisionsL; i++) {
			glm::vec3 point(
				radiusT * ((glm::sin(glm::radians(theta)) + 2.0f * sin(2 * glm::radians(theta)))) * 0.33f,
				radiusT * ((glm::cos(glm::radians(theta)) - 2.0f * cos(2 * glm::radians(theta)))) * 0.33f,
				radiusT * -glm::sin(3 * glm::radians(theta)) * 0.33f
			);
			points.push_back(point);

			theta += offsetL;
		}

		const GLfloat mPI = 3.142f;

		for (GLuint i = 0; i < divisionsL; i++) {

			GLuint prev = (i - 1) == -1 ? divisionsL - 1 : (i - 1);
			GLuint next = (i + 1) == divisionsL ? 0 : (i + 1);

			glm::vec3 v1(0.0f, 0.0f, 1.0f);
			glm::vec3 v2 = glm::normalize(glm::normalize(points[next] - points[i]) + glm::normalize(points[i] - points[prev]));

			GLfloat axAngle = 0.0f;
			GLfloat z = glm::length(v1) * glm::length(v2);
			if (z != 0) axAngle = glm::acos(glm::dot(v1, v2) / z);

			glm::quat q(glm::cos(axAngle / 2), glm::sin(axAngle / 2) * glm::cross(v1, v2));
			q = glm::normalize(q);

			GLfloat phi = 0.0f;

			for (GLuint j = 0; j < divisionsN; j++) {
				glm::vec3 pointR(
					radiusN * glm::cos(glm::radians(phi)),
					radiusN * glm::sin(glm::radians(phi)),
					0.0f
				);

				glm::mat4 rot = glm::toMat4(q);
				pointR = rot * glm::vec4(pointR, 1.0f);

				pointR += points[i];

				// vertices
				vertices[index++] = pointR.x;
				vertices[index++] = pointR.y;
				vertices[index++] = pointR.z;

				glm::vec3 normal = glm::normalize(pointR - points[i]);

				vertices[index++] = normal.x;
				vertices[index++] = normal.y;
				vertices[index++] = normal.z;

				phi += offsetN;
			}
		}

		// generate indices

		index = 0;

		for (GLuint i = 0; i < divisionsL; i++) {
			for (GLuint j = 0; j < divisionsN; j++) {
				indices[index++] = i * divisionsN + j;
				indices[index++] = i * divisionsN + ((j + 1 == divisionsN) ? 0 : (j + 1));
				indices[index++] = ((i + 1 == divisionsL) ? 0 : (i + 1)) * divisionsN + ((j + 1 == divisionsN) ? 0 : (j + 1));

				indices[index++] = i * divisionsN + j;
				indices[index++] = ((i + 1 == divisionsL) ? 0 : (i + 1)) * divisionsN + j;
				indices[index++] = ((i + 1 == divisionsL) ? 0 : (i + 1)) * divisionsN + ((j + 1 == divisionsN) ? 0 : (j + 1));
			}
		}
	}

public:
	GLuint divisionsL, divisionsN;
	GLfloat radiusN, radiusT;

	Trefoil(glm::vec3 position, GLuint divL, GLuint divN, GLfloat radN = 0.125f, GLfloat radT = 1.0f) : MeshObject(divL * divN, divL * divN * 2, 6) {
		this->position = position;
		this->divisionsL = divL;
		this->divisionsN = divN;

		this->radiusN = radN;
		this->radiusT = radT;

		GenerateVertices();

		BindBuffers();
	}
};
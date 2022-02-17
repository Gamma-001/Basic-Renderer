#include "include/3d_shapes.h"
#include "include/shader.hpp"
#include "include/mesh_object.hpp"
#include "include/empty_object.hpp"
#include "include/camera.hpp"

#include <iostream>
#include <vector>

static int WIN_WIDTH  = 800;
static int WIN_HEIGHT = 800;

struct Mouse {
	GLdouble x;
	GLdouble y;

	GLboolean middleButton;
} mouse;

Camera viewCam(glm::vec3(0.0f, 0.0f, -6.0f));

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_W:
		if (action == GLFW_REPEAT || action == GLFW_PRESS) {
			viewCam.TranslateLocal(0.1f, cameraDirection::front);
		}
		break;

	case GLFW_KEY_S:
		if (action == GLFW_REPEAT || action == GLFW_PRESS) {
			viewCam.TranslateLocal(-0.1f, cameraDirection::front);
		}
		break;

	case GLFW_KEY_D:
		if (action == GLFW_REPEAT || action == GLFW_PRESS) {
			viewCam.TranslateLocal(0.1f, cameraDirection::right);
		}
		break;

	case GLFW_KEY_A:
		if (action == GLFW_REPEAT || action == GLFW_PRESS) {
			viewCam.TranslateLocal(-0.1f, cameraDirection::right);
		}
		break;

	default:
		break;
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_MIDDLE:
		if (action == GLFW_PRESS)
			mouse.middleButton = GL_TRUE;
		else if (action == GLFW_RELEASE)
			mouse.middleButton = GL_FALSE;
		break;
	default:
		// do nothing
		break;
	}
}

void scrollCallback(GLFWwindow* window, double offsetX, double offsetY) {
	viewCam.Scale(offsetY * 0.125f + 1.0f);
}

void cursorPositionCallback(GLFWwindow* window, double posX, double posY) {
	float offsetX = posX - mouse.x;
	float offsetY = posY - mouse.y;

	mouse.x = posX;
	mouse.y = posY;

	if (mouse.middleButton) {
		viewCam.Rotate(offsetX * 0.25f, glm::vec3(0.0f, 0.0f, 1.0f));
		viewCam.RotateLocalX(offsetY * 0.25f);
	}
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
	WIN_WIDTH  = width;
	WIN_HEIGHT = height;

	if (WIN_HEIGHT == 0 || WIN_WIDTH == 0) { return; }

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.01f, 1000.0f);
	viewCam.projection_mat = projection;

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
}

int main() {
	
	// initialize glfw and set window hints
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 2);

	// create a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "3D Shapes", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "Failed to create a GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// initialize glew
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Faled to initialize glew" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	glfwGetCursorPos(window, &mouse.x, &mouse.y);
	mouse.middleButton = GL_FALSE;

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.01f, 1000.0f);
	viewCam.projection_mat = projection;

	viewCam.Rotate(-45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	viewCam.Rotate(-45.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	// meshes
	Disk disk(0.5f, 100);
	UVSphere sphere1(0.75f, glm::vec3(-2.0f, 0.0f, 0.0f), 128, 64);
	Torus torus(glm::vec3(2.0f, 0.0f, 0.0f), 0.20f, 0.5f, 32, 96);
	Trefoil trefoil(glm::vec3(0.0f, 0.0f, 0.0f), 256, 32, 0.17f);

	// empties
	Line yAxis(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f), 2.0f);
	Line xAxis(glm::vec3(-100.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), 2.0f);
	Grid floor(1.0f, 100, 0.5f);

	// shaders
	Shader defaultShader("./shaders/defaultVert.glsl", "./shaders/defaultFrag.glsl");

	Shader gridShader("./shaders/gridVert.glsl", "./shaders/gridFrag.glsl");
	
	Shader colorRed("./shaders/gridVert.glsl", "./shaders/gridFrag.glsl");
	Shader colorGreen("./shaders/gridVert.glsl", "./shaders/gridFrag.glsl");

	// modifications and other declarations
	disk.SetShader(defaultShader.Program);
	sphere1.SetShader(defaultShader.Program);
	torus.SetShader(defaultShader.Program);
	trefoil.SetShader(defaultShader.Program);

	yAxis.SetShader(colorGreen.Program, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	xAxis.SetShader(colorRed.Program, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	floor.SetShader(gridShader.Program, glm::vec4(0.7f, 0.7f, 0.7f, 0.25f));

	std::vector <glm::vec3> lightPos {
		glm::vec3(0.0f, 30.0f, 30.0f),
		glm::vec3(30.0f, -30.0f, 0.0f),
		glm::vec3(-30.0f, 0.0f, -30.0f)
	};

	glUseProgram(disk.shaderProgram);
		glUniform3f(glGetUniformLocation(trefoil.shaderProgram, "lightPosition[0]"), lightPos[0].x, lightPos[0].y, lightPos[0].z);
		glUniform3f(glGetUniformLocation(trefoil.shaderProgram, "lightPosition[1]"), lightPos[1].x, lightPos[1].y, lightPos[1].z);
		glUniform3f(glGetUniformLocation(trefoil.shaderProgram, "lightPosition[2]"), lightPos[2].x, lightPos[2].y, lightPos[2].z);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// game loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		floor.Draw(viewCam);

		trefoil.Draw(viewCam);
		sphere1.Draw(viewCam);
		torus.Draw(viewCam);

		yAxis.Draw(viewCam);
		xAxis.Draw(viewCam);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
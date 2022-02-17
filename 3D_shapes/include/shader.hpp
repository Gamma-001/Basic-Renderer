#pragma once
#pragma warning (disable : 26495)

#include "3d_shapes.h"

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
	GLuint Program;

	Shader(const char* vertShaderPath, const char* fragShaderPath, const char* geoShaderPath = "") {
		std::ifstream vertShaderSource, fragShaderSource, geoShaderSource;
		std::string vertShaderCode, fragShaderCode, geoShaderCode;

		// ensure that ifstream objects can throw an error
		vertShaderSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragShaderSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geoShaderSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		// read the shader files
		try {
			vertShaderSource.open(vertShaderPath);
			fragShaderSource.open(fragShaderPath);

			std::stringstream vertShaderStream, fragShaderStream;
			
			vertShaderStream << vertShaderSource.rdbuf();
			fragShaderStream << fragShaderSource.rdbuf();

			vertShaderSource.close();
			fragShaderSource.close();
			
			if (geoShaderPath != "") {
				geoShaderSource.open(geoShaderPath);
				std::stringstream geoShaderStream;

				geoShaderStream << geoShaderSource.rdbuf();
				geoShaderSource.close();

				geoShaderCode = geoShaderStream.str();
			}

			vertShaderCode = vertShaderStream.str();
			fragShaderCode = fragShaderStream.str();
		}
		catch(std::ifstream::failure &e) {
			std::cout << "ERROR::SHADER::IFSTREAM_FAILURE\n";
			std::cout << e.what() << std::endl;

			return;
		}

		// shader functions require a const char*
		const GLchar* vertCodeChars = vertShaderCode.c_str();
		const GLchar* fragCodeChars = fragShaderCode.c_str();
		const GLchar* geoCodeChars	= geoShaderCode.c_str();

		// compile the vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertCodeChars, nullptr);
		glCompileShader(vertexShader);

		// check for compilation errors
		{
			GLint success;
			GLchar infoLog[512];

			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

			if (!success) {
				glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
				
				std::cout << "ERROR::SHADER_COMPILATION::VERTEX_SHADER\n";
				std::cout << infoLog << std::endl;
				return;
			}
		}

		GLuint geoShader = 0;
		if (geoShaderPath != "") {
			geoShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geoShader, 1, &geoCodeChars, nullptr);
			glCompileShader(geoShader);

			GLint success;
			GLchar infoLog[512];

			glGetShaderiv(geoShader, GL_COMPILE_STATUS, &success);

			if (!success) {
				glGetShaderInfoLog(geoShader, 512, nullptr, infoLog);

				std::cout << "ERROR::SHADER_COMPILATION::GEOMETRY_SHADER\n";
				std::cout << infoLog << std::endl;

				return;
			}
		}

		// compile the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragCodeChars, nullptr);
		glCompileShader(fragmentShader);

		// check for compilaiton errors
		{
			GLint success;
			GLchar infoLog[512];

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

			if (!success) {
				glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);

				std::cout << "ERROR::SHADER_COMPILATION::FRAGMENT_SHADER\n";
				std::cout << infoLog << std::endl;

				return;
			}
		}

		// linke the compiled shaders to the shader program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertexShader);
		if (geoShaderPath != "") {
			glAttachShader(this->Program, geoShader);
		}
		glAttachShader(this->Program, fragmentShader);
		glLinkProgram(this->Program);

		// check for linkage errors
		{
			GLint success;
			GLchar infoLog[512];

			glGetProgramiv(this->Program, GL_LINK_STATUS, &success);

			if (!success) {
				glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
				
				std::cout << "ERROR::SHADER_PROGRAM_LINKAGE\n";
				std::cout << infoLog << std::endl;

				return;
			}
		}

		// delete the shaders since they are not needed anymore, in this use case
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
};
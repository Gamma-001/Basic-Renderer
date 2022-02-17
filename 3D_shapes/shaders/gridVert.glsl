#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;

void main() {
	fragPos = position;

	gl_Position = projection * view * vec4(position, 1.0f);
}
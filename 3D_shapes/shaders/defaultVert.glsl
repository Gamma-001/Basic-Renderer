#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;

uniform mat3 normal_mat;
uniform vec3 lightPosition[3];

out vec3 fragPos;
out vec3 vertNormal;
out vec3 lightPosView[3];

void main(){
	gl_Position  = projection * view * vec4(position, 1.0f);
	
	for (int i = 0; i < 3; i++) {
		lightPosView[i] = vec3(view * vec4(lightPosition[i], 1.0f));
	}

	fragPos = vec3(view * vec4(position, 1.0f));
	vertNormal = normalize(normal_mat * normal);
}
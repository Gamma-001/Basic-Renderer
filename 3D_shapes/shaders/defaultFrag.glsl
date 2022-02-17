#version 330 core

in vec3 fragPos;
in vec3 lightPosView[3];
in vec3 vertNormal;

uniform vec3 lightPosition[3];

out vec4 color;

void main() {
	float diff = 0.0f;
	float spec = 0.0f;

	// the lighting is calculated in view space
	for (int i = 0; i < 2; i += 1) {
		// all vectors are pointing outwards
		vec3 lightDir	= normalize(lightPosView[i] - fragPos);
		vec3 reflectDir	= reflect(lightDir, vertNormal);

		diff += max(dot(lightDir, vertNormal), 0.0f);
		spec += pow(max(dot(reflectDir, normalize(fragPos)), 0.0f), 64);
	}

	color = vec4(vec3(0.1f, 0.1f, 0.1f) + diff * vec3(0.8f, 0.8f, 0.8f) + spec * vec3(1.0f, 1.0f, 1.0f), 1.0f);
}
#version 330 core

in vec3 fragPos;

uniform vec4 vertColor;
uniform float worldScale;

out vec4 color;

void main() {
	float dist = length(fragPos);
	float alphaVal = 1.0f;

	if (dist > 100.0f) alphaVal = 0.0f;
	else {
		dist = 1.571f * dist / 100.0f;
		alphaVal = pow(worldScale, 0.25) * pow(cos(dist), 2);
	}

	color = vec4(vec3(vertColor), 0.75f * alphaVal);
}
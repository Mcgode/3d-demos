#version 330 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec3 inputNormal;
layout (location = 2) in vec2 inputTextureCoordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMat;

out vec3 normal;
out vec3 worldPosition;
out vec2 textureCoordinates;

void main() {
    worldPosition = (model * vec4(inputPosition, 1)).xyz;
    normal = normalMat * inputNormal;
    textureCoordinates = inputTextureCoordinates;
	gl_Position = projection * view * model * vec4(inputPosition, 1.0);
}
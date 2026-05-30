#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uOffset;

out vec3 vNormal;

void main() {
	gl_Position = uProjection * uView * vec4(aPos + uOffset, 1.0);
	vNormal = aNormal;
}
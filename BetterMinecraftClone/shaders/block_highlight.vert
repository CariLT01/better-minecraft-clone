#version 460

layout (location = 0) in vec3 aPos;

uniform vec3 uOffset;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {

	vec3 normalizedPos = (aPos + vec3(1.0)) * 0.5;
	vec3 centered = normalizedPos - vec3(0.5);
	vec3 enlarged = centered * 1.01;
	vec3 finalPos = enlarged + vec3(0.5);

	gl_Position = uProjection * uView * vec4(finalPos + uOffset, 1.0);
}
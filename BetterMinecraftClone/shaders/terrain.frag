#version 460 core

uniform sampler2DArray uTextureAtlas;

in float vBrightness;
in vec2 vUv;
in float vBlockLight;
in float vSkyLight;
flat in int fBlockType;

out vec4 FragColor;

void main() {


	float blockLighting = max(vBlockLight / 15.0, vSkyLight / 15.0);

	vec3 blockColor = texture(uTextureAtlas, vec3(vUv, fBlockType)).rgb; 



	FragColor = vec4(blockColor * vec3(vBrightness * blockLighting), 1.0);
}
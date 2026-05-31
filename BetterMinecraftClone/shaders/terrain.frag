#version 460 core

uniform sampler2DArray uTextureAtlas;

in float vBrightness;
in vec2 vUv;
flat in int fBlockType;

out vec4 FragColor;

void main() {


	vec3 blockColor = texture(uTextureAtlas, vec3(vUv, fBlockType)).rgb; 



	FragColor = vec4(blockColor * vec3(vBrightness), 1.0);
}
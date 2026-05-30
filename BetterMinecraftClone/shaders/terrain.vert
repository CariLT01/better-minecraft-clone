#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in int aBlockType;

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uOffset;

out float vBrightness;
out vec2 vUv;
flat out int fBlockType;

void main() {



	gl_Position = uProjection * uView * vec4(aPos + uOffset, 1.0);

    // 2. Extract absolute components of the normal
    vec3 absNorm = abs(aNormal);
    float brightness = 0.0;

    // 3. Dot product blend to select the right multiplier without using 'if' statements.
    // If vNormal points up (0, 1, 0), absNorm.y is 1.0, others are 0.0, so brightness becomes 1.0.
    if (aNormal.y > 0.0) {
        brightness = 1.0;                   // Top face (Y+)
    } else if (aNormal.y < 0.0) {
        brightness = 0.5;                   // Bottom face (Y-)
    } else {
        // Blend between X and Z faces based on which one is active
        brightness = (absNorm.x * 0.6) + (absNorm.z * 0.8);
    }
    vBrightness = brightness;
    vUv = aUv;
    fBlockType = aBlockType;


}
#version 460 core

in vec3 vNormal;

out vec4 FragColor;

void main() {


	vec3 blockColor = vec3(1.0, 1.0, 1.0); 

    // 2. Extract absolute components of the normal
    vec3 absNorm = abs(vNormal);
    float brightness = 0.0;

    // 3. Dot product blend to select the right multiplier without using 'if' statements.
    // If vNormal points up (0, 1, 0), absNorm.y is 1.0, others are 0.0, so brightness becomes 1.0.
    if (vNormal.y > 0.0) {
        brightness = 1.0;                   // Top face (Y+)
    } else if (vNormal.y < 0.0) {
        brightness = 0.5;                   // Bottom face (Y-)
    } else {
        // Blend between X and Z faces based on which one is active
        brightness = (absNorm.x * 0.6) + (absNorm.z * 0.8);
    }

	FragColor = vec4(vec3(brightness), 1.0);
}
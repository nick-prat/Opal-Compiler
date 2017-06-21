#version 450

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D gSampler;
uniform vec4 gAmbientLight;

void main() {
    FragColor = texture2D(gSampler, texCoord) * vec4(gAmbientLight.xyz, 1.0f) * gAmbientLight.w;
}

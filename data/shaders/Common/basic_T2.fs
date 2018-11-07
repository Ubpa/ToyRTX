#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;

void main(){
	FragColor = vec4(vec3(texture(texture0, TexCoords)), 1.0f);
}
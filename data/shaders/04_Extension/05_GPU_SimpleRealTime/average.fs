#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;
uniform int num;

void main(){
	FragColor = texture(texture0, TexCoords).xyz / num;
}
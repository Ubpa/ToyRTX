#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D SumColor;

void main(){
	vec4 sumColor = texture(SumColor, TexCoords);
	FragColor = sumColor.w == 0 ? vec3(0) : sumColor.xyz / sumColor.w;
}
#version 330 core
layout (location = 0) out vec4 out_origin_curRayNum;
layout (location = 1) out vec4 out_dir_tMax;
layout (location = 2) out vec4 out_color_time;
layout (location = 3) out vec3 out_rayTracingRst;

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;
uniform sampler2D color_time;
uniform sampler2D rayTracingRst;
uniform float RayNumMax;

in vec2 TexCoords;

void main(){
	float curRayNum = texture(origin_curRayNum, TexCoords).w;
	out_origin_curRayNum = vec4(vec3(0),min(curRayNum+1.0,RayNumMax));
	out_rayTracingRst = curRayNum/RayNumMax*vec3(TexCoords,1.0);
}

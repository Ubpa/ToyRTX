#include <RayTracing/TRayCamera.h>

using namespace RayTracing;
using namespace glm;

TRayCamera::TRayCamera(const vec3 & pos, const vec3 & viewPoint, float ratioWH,
	float t0, float t1, float lenR,
	float fov, float focus_dist, const vec3 & worldUp)
	: RayCamera(pos,viewPoint,ratioWH,fov,focus_dist,worldUp),
	t0(t0), t1(t1), lenR(lenR){
}

TRay TRayCamera::GenTRay(float s, float t) {
	vec2 rd = lenR * RandomInUnitCircle();
	vec3 offset = right * rd.x + up * rd.y;
	vec3 origin = pos + offset;
	float time = t0 + GetRand01()*(t1 - t0);
	vec3 dir = BL_Corner + s * horizontal + t * vertical - origin;
	return TRay(origin, dir, time);
}

float TRayCamera::GetRand01() {
	return rand() / (float)RAND_MAX;
}

vec2 TRayCamera::RandomInUnitCircle() {
	vec2 p;
	do {
		p = 2.0f * vec2(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX) - vec2(1, 1);
	} while (dot(p, p) >= 1.0f);
	return p;
}
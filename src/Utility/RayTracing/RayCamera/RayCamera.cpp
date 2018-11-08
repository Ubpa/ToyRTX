#include <RayTracing/RayCamera.h>

using namespace RayTracing;
using namespace glm;

const float RayCamera::PI = 3.1415926f;

RayCamera::RayCamera(const vec3 & pos, const vec3 & viewPoint, float ratioWH,
	float fov, float focus_dist, const vec3 & worldUp)
	:
	pos(pos)
{
	if (focus_dist == -1.0f)
		focus_dist = distance(viewPoint, pos);

	float theta = fov / 180.0f * PI;
	float height = 2 * focus_dist * tan(theta / 2);
	float width = ratioWH * height;
	front = normalize(viewPoint - pos);
	right = normalize(cross(worldUp, -front));
	up = cross(-front, right);
	BL_Corner = pos + focus_dist * front - width / 2 * right - height / 2 * up;
	horizontal = width * right;
	vertical = height * up;
}

Ray RayCamera::GenRay(float s, float t) {
	vec3 dir = BL_Corner + s * horizontal + t * vertical - pos;
	return Ray(pos, dir);
}
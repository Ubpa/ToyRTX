#include <RayTracing/RayCamera.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

const float RayCamera::PI = 3.1415926f;

RayCamera::RayCamera(const vec3 & pos, const vec3 & viewPoint, float ratioWH, float lenR,
	float fov, float focus_dist, const vec3 & worldUp)
	:
	pos(pos), lenR(lenR)
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

Ray::Ptr RayCamera::GenRay(float s, float t) const {
	vec2 rd = lenR * Math::RandInCircle();
	vec3 origin = pos + rd.x * right + rd.y * up;
	vec3 dir = BL_Corner + s * horizontal + t * vertical - origin;
	return ToPtr(new Ray(origin, dir));
}
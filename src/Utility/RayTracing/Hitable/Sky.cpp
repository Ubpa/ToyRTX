#include <RayTracing/Sky.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

HitRst Sky::RayIn(Ray::Ptr & ray) const {
	if (ray->GetTMax() != FLT_MAX)
		return HitRst::FALSE;

	HitRst hitRst(true);
	vec3 normal = normalize(ray->GetDir());
	hitRst.record = HitRecord(ray, normal, -normal);
	hitRst.hitable = this;
	hitRst.isMatCoverable = isMatCoverable;

	return hitRst;
}

AABB Sky::GetBoundingBox() const {
	return { vec3(10e-20), vec3(10e20) };
}
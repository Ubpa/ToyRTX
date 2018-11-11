#include <RayTracing/Sky.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Hitable::HitRst Sky::RayIn(Ray::Ptr & ray) const {
	if (ray->GetTMax() != FLT_MAX)
		return HitRst::FALSE;

	HitRst hitRst(true);
	vec3 normal = normalize(ray->GetDir());
	hitRst.record = HitRecord(ray, normal, -normal);
	hitRst.hitable = this;

	return hitRst;
}
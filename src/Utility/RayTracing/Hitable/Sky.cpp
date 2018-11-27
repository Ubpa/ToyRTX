#include <RayTracing/Sky.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

const AABB Sky::box(vec3(10e-20f), vec3(10e20f));

HitRst Sky::RayIn(Ray::Ptr & ray) const {
	if (ray->GetTMax() != FLT_MAX)
		return HitRst::FALSE;

	HitRst hitRst(true);
	vec3 normal = normalize(ray->GetDir());
	hitRst.record = HitRecord(ray, normal, -normal);
	hitRst.material = GetMat();
	hitRst.isMatCoverable = IsMatCoverable();

	return hitRst;
}
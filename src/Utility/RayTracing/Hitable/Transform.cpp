#include <RayTracing/Transform.h>

using namespace RayTracing;
using namespace glm;

Transform::Transform(const glm::mat4 & transform, const Hitable::Ptr & hitable)
	: transform(transform), hitable(hitable) { }

Hitable::HitRst Transform::RayIn(Ray::Ptr & ray) const {
	if (hitable == NULL)
		return HitRst::FALSE;

	ray->SetTransform(inverse(transform));
	auto hitRst = hitable->RayIn(ray);
	ray->SetTransform(transform);

	return hitRst;
}

AABB Transform::GetBoundingBox() const {
	if (hitable == NULL)
		return AABB::InValid;

	return hitable->GetBoundingBox();
}
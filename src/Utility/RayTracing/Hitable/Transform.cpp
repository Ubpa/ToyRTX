#include <RayTracing/Transform.h>

using namespace RayTracing;
using namespace glm;

Transform::Transform(const mat4 & transform, const Hitable::CPtr & hitable, const Material::CPtr & material)
	: transform(transform), hitable(hitable), Hitable(material) { }

HitRst Transform::RayIn(Ray::Ptr & ray) const {
	if (hitable == NULL)
		return HitRst::FALSE;

	ray->Transform(inverse(transform));

	auto hitRst = hitable->RayIn(ray);

	if (hitRst.hit) {
		if (hitRst.isMatCoverable && material != NULL) {
			hitRst.material = material;
			hitRst.isMatCoverable = isMatCoverable;
		}

		hitRst.record.vertex.Transform(transform);
	}

	ray->Transform(transform);

	return hitRst;
}

AABB Transform::GetBoundingBox() const {
	if (hitable == NULL)
		return AABB::InValid;

	return hitable->GetBoundingBox();
}
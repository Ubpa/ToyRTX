#include <RayTracing/Group.h>

using namespace RayTracing;
using namespace CppUtility::Other;

Group::Group(const Material::CPtr & material)
	: Hitable(material), box(AABB::InValid) { }

HitRst Group::RayIn(Ray::Ptr & ray) const {
	HitRst finalHitRst = HitRst::FALSE;

	for (size_t i = 0; i < children.size(); i++) {
		Hitable::CPtr hitable = children[i];
		HitRst hitRst = hitable->RayIn(ray);

		if (hitRst.hit)
			finalHitRst = hitRst;

	}

	if (finalHitRst.hit && finalHitRst.isMatCoverable && material != NULL) {
		finalHitRst.material = material;
		finalHitRst.isMatCoverable = isMatCoverable;
	}

	return finalHitRst;
}

Group & Group::operator <<(const Hitable::CPtr & hitable) {
	if (hitable != NULL) {
		children.push_back(hitable);
		box += hitable->GetBoundingBox();
	}
	
	return *this;
}
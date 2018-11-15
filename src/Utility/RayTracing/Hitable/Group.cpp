#include <RayTracing/Group.h>

using namespace RayTracing;
using namespace CppUtility::Other;

HitRst Group::RayIn(Ray::Ptr & ray) const {
	Hitable::Ptr cloestObj = NULL;
	HitRst finalHitRst = HitRst::FALSE;

	for (auto & hitable : *this) {
		HitRst hitRst = hitable->RayIn(ray);

		if (hitRst.hit) {
			cloestObj = hitable;
			finalHitRst = hitRst;
		}
	}
	if (finalHitRst.hit && finalHitRst.isMatCoverable && material != NULL) {
		finalHitRst.material = material;
		finalHitRst.isMatCoverable = isMatCoverable;
	}

	return finalHitRst;
}

Group & Group::operator <<(const Hitable::Ptr & hitable) {
	if (hitable != NULL)
		push_back(hitable);
	
	return *this;
}

AABB Group::GetBoundingBox() const {
	if (size() == 0)
		return AABB::InValid;

	AABB rst = at(0)->GetBoundingBox();
	for (size_t i = 1; i < size(); i++)
		rst += at(i)->GetBoundingBox();

	return rst;
}
#include <RayTracing/Group.h>

using namespace RayTracing;
using namespace CppUtility::Other;

HitRst Group::RayIn(Ray::Ptr & ray) const {
	HitRst finalHitRst = HitRst::FALSE;

	for (size_t i = 0; i < size(); i++) {
		Hitable::CPtr hitable = at(i);
		HitRst hitRst = hitable->RayIn(ray);

		if (hitRst.hit)
			finalHitRst = hitRst;

	}
	//for (auto & hitable : *this) {
	//}
	if (finalHitRst.hit && finalHitRst.isMatCoverable && material != NULL) {
		finalHitRst.material = material;
		finalHitRst.isMatCoverable = isMatCoverable;
	}

	return finalHitRst;
}

Group & Group::operator <<(const Hitable::CPtr & hitable) {
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
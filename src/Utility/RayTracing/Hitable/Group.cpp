#include <RayTracing/Group.h>

using namespace RayTracing;
using namespace CppUtility::Other;

Group::Group() { }

Hitable::HitRst Group::RayIn(Ray::Ptr & ray) const {
	Hitable::Ptr cloestObj = NULL;
	HitRst finalHitRst = HitRst::FALSE;

	for (auto & hitable : *this) {
		HitRst hitRst = hitable->RayIn(ray);
		if (hitRst.hit) {
			cloestObj = hitable;
			finalHitRst = hitRst;
		}
	}

	return finalHitRst;
}

Group & Group::operator <<(const CppUtility::Other::Ptr<Hitable> & hitable) {
	if(hitable != false)
		push_back(hitable);
	
	return *this;
}

AABB Group::BoundingBox() const {
	if (size() == 0)
		return AABB::InValid;

	AABB rst = at(0)->BoundingBox();
	for (size_t i = 1; i < size(); i++)
		rst += at(i)->BoundingBox();

	return rst;
}
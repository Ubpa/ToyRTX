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
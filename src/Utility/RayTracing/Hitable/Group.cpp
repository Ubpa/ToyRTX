#include <RayTracing/Group.h>

using namespace RayTracing;
using namespace CppUtility::Other;

Group::Group() { }

Group::~Group() { }

bool Group::Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
	HitRecord tmpRec;
	bool hit_anything = false;
	float closest_so_far = t_max;

	for (int i = 0; i < size(); i++) {
		if (at(i)->Hit(ray, t_min, closest_so_far, tmpRec)) {
			hit_anything = true;
			closest_so_far = tmpRec.t;
		}
	}

	if (hit_anything)
		rec = tmpRec;

	return hit_anything;
}

Group & Group::operator <<(const Ptr<Hitable> & hitable) {
	if(hitable != NULL)
		push_back(hitable);

	return *this;
}
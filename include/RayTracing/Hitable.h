#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <RayTracing/Ray.h>

namespace RayTracing {
	class Hitable {
	public:
		struct HitRecord{
			float t;
			glm::vec3 p;
		};

		virtual bool Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const = 0;
	};
}
#endif
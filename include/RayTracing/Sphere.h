#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Sphere : public Hitable {
	public:
		Sphere(const glm::vec3 & center, float radius);
		virtual bool Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;

		glm::vec3 center;
		float radius;
	};
}

#endif // !_SPHERE_H_

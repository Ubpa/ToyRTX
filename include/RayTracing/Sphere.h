#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Sphere : public Hitable {
		HEAP_OBJ_SETUP(Sphere)
	public:
		Sphere(const glm::vec3 & center, float radius, Material::Ptr material);

		virtual Hitable::HitRst RayIn(Ray::Ptr & ray) const;
	protected:

		glm::vec3 center;
		float radius;
	};
}

#endif // !_SPHERE_H_

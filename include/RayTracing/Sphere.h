#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Sphere : public Hitable {
		HEAP_OBJ_SETUP(Sphere)
	public:
		Sphere(const glm::vec3 & center = glm::vec3(0.0f), float radius = 1.0f, Material::Ptr material = NULL);

		virtual Hitable::HitRst RayIn(Ray::Ptr & ray) const;
	protected:

		glm::vec3 center;
		float radius;
	};
}

#endif // !_SPHERE_H_

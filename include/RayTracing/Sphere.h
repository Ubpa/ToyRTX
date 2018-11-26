#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Sphere : public Hitable {
		HITABLE_SETUP(Sphere)
	public:
		Sphere(const glm::vec3 & center = glm::vec3(0.0f), float radius = 1.0f, const Material::CPtr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const;
		virtual const AABB GetBoundingBox() const { return box; }
	protected:

		glm::vec3 center;
		float radius;
		AABB box;
	};
}

#endif // !_SPHERE_H_

#ifndef _MOVE_SPHERE_H_
#define _MOVE_SPHERE_H_

#include <RayTracing/TRay.h>
#include <RayTracing/Sphere.h>

namespace RayTracing {
	class MoveSphere : public Sphere{
		HEAP_OBJ_SETUP(MoveSphere)
	public:
		MoveSphere(float t0, float t1, const glm::vec3 & center0, const glm::vec3 & center1, float radius, Material::Ptr material);

		virtual HitRst RayIn(Ray::Ptr & ray) const;

		virtual AABB GetBoundingBox() const;
	private:
		glm::vec3 center0;
		glm::vec3 center1;
		float t0;
		float t1;
	};
}

#endif // !_MOVE_SPHERE_H_

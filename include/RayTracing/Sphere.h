#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Sphere : public Hitable {
	public:
		Sphere(const glm::vec3 & center, float radius, Material * material);
		virtual bool Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
	protected:
		virtual ~Sphere();
	private:
		glm::vec3 center;
		float radius;
		Material * material;
	};
}

#endif // !_SPHERE_H_

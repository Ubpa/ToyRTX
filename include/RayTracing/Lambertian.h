#ifndef _LAMBERTIAN_H_
#define _LAMBERTIAN_H_

#include <RayTracing/Material.h>

namespace RayTracing {
	class Lambertian : public Material {
	public:
		Lambertian(const glm::vec3 & albedo);

		bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& rayOut) const;
	protected:
		vec3 albedo;
	};
}

#endif // !_LAMBERTIAN_H_

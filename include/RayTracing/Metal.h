#ifndef _METAL_H_
#define _METAL_H_

#include <RayTracing/Material.h>

namespace RayTracing {
	class Metal : public Material{
	public:
		Metal(const glm::vec3 & specular);
		bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& rayOut) const;
	protected:
		glm::vec3 specular;
	};
}

#endif // !_METAL_H_

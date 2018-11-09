#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <RayTracing/Ray.h>
#include <RayTracing/Hitable.h>
#include <glm/glm.hpp>

namespace RayTracing {
	class Material {
	public:
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& rayOut) const = 0;
		virtual glm::vec3 Emit(float u, float v, const glm::vec3& p) const;
	};
}

#endif // !_MATERIAL_H_

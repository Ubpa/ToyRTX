#ifndef _LAMBERTIAN_H_
#define _LAMBERTIAN_H_

#include <RayTracing/Material.h>

namespace RayTracing {
	class Lambertian : public Material {
		HEAP_OBJ_SETUP(Lambertian)
	public:
		Lambertian(float r, float g, float b);
		Lambertian(const glm::vec3 & albedo);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(HitRecord & rec) const;
	protected:

		glm::vec3 albedo;
	};
}

#endif // !_LAMBERTIAN_H_

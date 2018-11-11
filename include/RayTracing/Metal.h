#ifndef _METAL_H_
#define _METAL_H_

#include <RayTracing/Material.h>

namespace RayTracing {
	class Metal : public Material{
		HEAP_OBJ_SETUP(Metal)
	public:
		Metal(float r, float g, float b, float fuzz = 0.0f);
		Metal(const glm::vec3 & specular, float fuzz = 0.0f);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(HitRecord & rec) const;
	protected:
		glm::vec3 specular;
		float fuzz;
	};
}

#endif // !_METAL_H_

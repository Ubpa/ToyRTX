#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <RayTracing/Ray.h>

#include <Utility/HeapObj.h>

#include <glm/glm.hpp>

#include <vector>

namespace RayTracing {
	struct HitRecord {
		HitRecord(const Ray::Ptr & ray = NULL, const glm::vec3 & pos = glm::vec3(0), const glm::vec3 & normal = glm::vec3(0,0,1));

		Ray::Ptr ray;
		float u;
		float v;
		glm::vec3 pos;
		glm::vec3 normal;
	};

	class Material : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Material)
	public:
		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(HitRecord & rec) const = 0;
	};
}

#endif // !_MATERIAL_H_

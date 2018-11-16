#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Transform : public Hitable {
		HEAP_OBJ_SETUP(Transform)
	public:
		// 如果 material 不为空, 则覆盖子节点的 material
		Transform(const glm::mat4 & transform, const Hitable::CPtr & hitable, const Material::CPtr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const;
		virtual AABB GetBoundingBox() const;
	private:
		glm::mat4 transform;
		Hitable::CPtr hitable;
		AABB box;
	};
}

#endif // !_TRANSFORM_H_

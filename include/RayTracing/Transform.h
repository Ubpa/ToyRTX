#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Transform : public Hitable {
		HITABLE_SETUP(Transform)
	public:
		// 如果 material 不为空, 则覆盖子节点的 material
		Transform(const glm::mat4 & transform, const Hitable::CPtr & hitable, const Material::CPtr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const;
		virtual const AABB GetBoundingBox() const { return box; }
		const glm::mat4 GetTransform() const { return transform; }
		const glm::mat4 GetInvTransform() const { return inverseTransform; }
		const glm::mat3 GetNormTransform() const { return normalTransform; }
		const Hitable::CPtr GetChild() const { return hitable; }
	private:
		//如果要修改transform, 则要同时设置 inverseTransform 和 normalTransform
		glm::mat4 transform;
		glm::mat4 inverseTransform;
		glm::mat3 normalTransform;

		Hitable::CPtr hitable;
		AABB box;
	};
}

#endif // !_TRANSFORM_H_

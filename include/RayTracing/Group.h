#ifndef _GROUP_H_
#define _GROUP_H_

#include <RayTracing/Hitable.h>
#include <vector>

namespace RayTracing {
	class Group : public Hitable, private std::vector<Hitable::CPtr>{
		HEAP_OBJ_SETUP(Group)
	public:
		Group(const Material::CPtr & material = NULL);

		// 返回 true 说明光线继续传播
		// 返回 false 说明光线传播结束
		virtual HitRst RayIn(Ray::Ptr & ray) const;
		inline void push_back(const Hitable::CPtr & hitable) { (*this) << hitable; }
		Group & operator <<(const Hitable::CPtr & hitable);
		virtual const AABB GetBoundingBox() const { return box; }

	private:
		AABB box;
	};
}
#endif //!_GROUP_H_


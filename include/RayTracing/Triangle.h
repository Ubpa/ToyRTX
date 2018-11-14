#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Triangle : public Hitable {
		HEAP_OBJ_SETUP(Triangle)
	public:
		Triangle(const Vertex & A, const Vertex & B, const Vertex & C, const Material::Ptr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const;
		virtual AABB GetBoundingBox() const;
	private:
		Vertex A, B, C;
	};
}

#endif // !_TRIANGLE_H_

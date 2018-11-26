#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Triangle : public Hitable {
		HITABLE_SETUP(Triangle)
	public:
		Triangle(const Vertex & A, const Vertex & B, const Vertex & C, const Material::CPtr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const;
		virtual const AABB GetBoundingBox() const { return box; }
	private:
		Vertex A, B, C;
		AABB box;
	};
}

#endif // !_TRIANGLE_H_

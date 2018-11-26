#ifndef _SKY_H_
#define _SKY_H_

#include <RayTracing/Hitable.h>

namespace RayTracing {
	class Sky : public Hitable {
		HITABLE_SETUP(Sky)
	public:
		using Hitable::Hitable;

		virtual HitRst RayIn(Ray::Ptr & ray) const;

		virtual const AABB GetBoundingBox() const { return box; }
	private:
		static const AABB box;
	};
}

#endif // !_SKY_H_

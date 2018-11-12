#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <RayTracing/AABB.h>
#include <RayTracing/Ray.h>
#include <RayTracing/Material.h>

#include <Utility/HeapObj.h>

namespace RayTracing {

	class Hitable : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Hitable)
	public:

		struct HitRst{
			HitRst(bool hit = false) : hit(hit) { }

			bool hit;
			HitRecord record;
			const Hitable * hitable;

			//------------
			static const HitRst FALSE;
		};


		Hitable(const Material::Ptr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const = 0;
		bool RayOut(HitRecord & rec) const;
		virtual AABB BoundingBox() const = 0;
	protected:
		const Material::Ptr material;
	};
}
#endif
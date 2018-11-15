#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <RayTracing/AABB.h>
#include <RayTracing/Ray.h>
#include <RayTracing/Material.h>

#include <Utility/HeapObj.h>

namespace RayTracing {
	class Hitable;

	struct HitRst {
		HitRst(bool hit = false) : hit(hit), hitable(NULL), isMatCoverable(true), material(NULL) { }

		bool hit;
		HitRecord record;
		CppUtility::Other::CPtr<Material> material;
		const Hitable * hitable;
		bool isMatCoverable;
		//------------
		static const HitRst FALSE;
	};

	class Hitable : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Hitable)
	public:
		Hitable(const Material::Ptr & material = NULL);

		virtual HitRst RayIn(Ray::Ptr & ray) const = 0;
		virtual AABB GetBoundingBox() const = 0;

		//bool RayOut(HitRecord & rec) const;
		//void SetMaterial(const Material::Ptr & material);
	protected:
		Material::CPtr material;
		bool isMatCoverable;
	};
}
#endif
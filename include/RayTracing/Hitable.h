#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <RayTracing/AABB.h>
#include <RayTracing/Ray.h>
#include <RayTracing/Material.h>

#include <RayTracing/HitableVisitor.h>

#define HITABLE_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(HitableVisitor * hitableVisitor) const{\
	hitableVisitor->Visit(this);\
}


namespace RayTracing {
	class Hitable;

	struct HitRst {
		HitRst(bool hit = false) : hit(hit), isMatCoverable(true), material(NULL) { }

		bool hit;
		HitRecord record;
		CppUtility::Other::CPtr<Material> material;
		bool isMatCoverable;
		//------------
		static const HitRst FALSE;
	};

	class Hitable : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Hitable)
	public:
		Hitable(const Material::CPtr & material = NULL);

		const Material::CPtr GetMat() const { return material; };

		virtual HitRst RayIn(Ray::Ptr & ray) const = 0;
		virtual const AABB GetBoundingBox() const = 0;
		virtual void Accept(HitableVisitor * hitableVisitor) const = 0;
	protected:
		Material::CPtr material;
		bool isMatCoverable;
	};
}
#endif
#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <RayTracing/AABB.h>
#include <RayTracing/Ray.h>
#include <RayTracing/Material.h>

#include <RayTracing/HitableVisitor.h>

#define HITABLE_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(const HitableVisitor::Ptr & hitableVisitor) const{\
	hitableVisitor->Visit(CThis());\
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
		bool IsMatCoverable() const { return isMatCoverable; }

		virtual HitRst RayIn(Ray::Ptr & ray) const = 0;
		virtual const AABB GetBoundingBox() const = 0;
		virtual void Accept(const HitableVisitor::Ptr & hitableVisitor) const = 0;
	private:
		Material::CPtr material;
		bool isMatCoverable;
	};
}
#endif
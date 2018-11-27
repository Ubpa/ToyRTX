#ifndef _MAT_VISITOR_H_
#define _MAT_VISITOR_H_

#include <Utility/HeapObj.h>

namespace RayTracing {
	class Lambertian;
	class Metal;
	class Dielectric;
	class Light;
	class OpMaterial;
	class Isotropic;

	class MatVisitor : public CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(MatVisitor)
	public:
		virtual void Visit(const CppUtility::Other::CPtr<Lambertian> & Lambertian);
		virtual void Visit(const CppUtility::Other::CPtr<Metal> & Metal);
		virtual void Visit(const CppUtility::Other::CPtr<Dielectric> & Dielectric);
		virtual void Visit(const CppUtility::Other::CPtr<Light> & Light);
		virtual void Visit(const CppUtility::Other::CPtr<OpMaterial> & OpMaterial);
		virtual void Visit(const CppUtility::Other::CPtr<Isotropic> & Isotropic);
	};
}

#endif // !_MAT_VISITOR_H_

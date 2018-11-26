#ifndef _MAT_VISITOR_H_
#define _MAT_VISITOR_H_

namespace RayTracing {
	class Lambertian;
	class Metal;
	class Dielectric;
	class Light;
	class OpMaterial;
	class Isotropic;

	class MatVisitor{
	public:
		virtual void Visit(const Lambertian * Lambertian);
		virtual void Visit(const Metal * Metal);
		virtual void Visit(const Dielectric * Dielectric);
		virtual void Visit(const Light * Light);
		virtual void Visit(const OpMaterial * OpMaterial);
		virtual void Visit(const Isotropic * Isotropic);
	};
}

#endif // !_MAT_VISITOR_H_

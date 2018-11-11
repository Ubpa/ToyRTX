#ifndef _OP_MATERIAL_H_
#define _OP_MATERIAL_H_

#include <functional>

#include <RayTracing/Material.h>

namespace RayTracing {
	class OpMaterial : public Material{
		HEAP_OBJ_SETUP(OpMaterial)
	public:
		OpMaterial(const std::function<bool(HitRecord & rec)> & op);

		virtual bool Scatter(HitRecord & rec) const;
	protected:
		std::function<bool(HitRecord & rec)> op;
	};
}

#endif // !_OP_MATERIAL_H_

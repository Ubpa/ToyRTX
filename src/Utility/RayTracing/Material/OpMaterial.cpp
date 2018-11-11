#include <RayTracing/OpMaterial.h>

using namespace RayTracing;
using namespace glm;

OpMaterial::OpMaterial(const std::function<bool(HitRecord & rec)> & op)
	: op(op){ }

bool OpMaterial::Scatter(HitRecord & rec) const {
	if (op == NULL)
		return false;

	return op(rec);
}
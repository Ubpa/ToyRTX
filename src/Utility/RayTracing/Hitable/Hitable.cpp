#include <RayTracing/Hitable.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

const Hitable::HitRst Hitable::HitRst::FALSE(false);

Hitable::Hitable(const Material::Ptr & material)
	: material(material) { }

bool Hitable::RayOut(HitRecord & rec) const {
	if (material == NULL)
		return false;

	return material->Scatter(rec);
}
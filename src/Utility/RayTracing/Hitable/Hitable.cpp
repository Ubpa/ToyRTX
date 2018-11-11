#include <RayTracing/Hitable.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

const float Hitable::tMin = 10e-4f;

const Hitable::HitRst Hitable::HitRst::FALSE(false);

Hitable::Hitable(const Material::Ptr & material)
	: material(material) { }

bool Hitable::RayOut(HitRecord & rec) const {
	if (material == NULL)
		return false;
	/*
	if (material->Scatter(rec)) {
		if (Math::Gray(rec.ray->GetColor()) >= 1.0f/256.0f)
			return true;
	}

	return false;
	*/
	return material->Scatter(rec);
}
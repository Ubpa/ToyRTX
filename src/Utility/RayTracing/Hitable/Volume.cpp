#include <RayTracing/Volume.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Volume::Volume(const Hitable::CPtr & boundary, float density, const Material::CPtr & material)
	: Hitable(material), density(density), boundary(boundary) { }

HitRst Volume::RayIn(Ray::Ptr & ray) const {
	if (boundary == NULL)
		return HitRst::FALSE;

	float originTMax = ray->GetTMax();

	auto hitRst = boundary->RayIn(ray);
	if (!hitRst.hit)
		return HitRst::FALSE;

	auto reverseRay = ToPtr(new Ray(ray->GetOrigin(), -ray->GetDir()));
	auto reverseHitRst = boundary->RayIn(reverseRay);
	
	float t0 = reverseHitRst.hit ? - hitRst.record.ray->GetTMax() : hitRst.record.ray->GetTMax();

	auto t0Ray = ToPtr(new Ray(ray->At(t0), ray->GetDir()));
	auto t0HitRst = boundary->RayIn(t0Ray);

	//Ì«±¡
	if (!t0HitRst.hit) {
		ray->SetTMax(originTMax);
		return HitRst::FALSE;
	}

	float t1 = min(originTMax, t0HitRst.record.ray->GetTMax());
	float lenInVolume = (t1 - t0) * length(ray->GetDir());

	// p = C * dL
	// p(L) = lim(n->inf, (1 - CL/n)^n) = exp(-CL)
	// L = -(1/C)ln(pL)
	float hitLen = -(1.0 / density)*log(Math::Rand_F());

	if (hitLen >= lenInVolume) {
		ray->SetTMax(originTMax);
		return HitRst::FALSE;
	}

	float tFinal = hitLen / length(ray->GetDir());
	ray->SetTMax(tFinal);

	hitRst.hit = true;
	hitRst.hitable = this;
	hitRst.record = HitRecord(ray, ray->At(tFinal));
	if (hitRst.isMatCoverable && material != NULL) {
		hitRst.material = material;
		hitRst.isMatCoverable = isMatCoverable;
	}
	return hitRst;
}

AABB Volume::GetBoundingBox() const {
	if (boundary == NULL)
		return AABB::InValid;

	return boundary->GetBoundingBox();
}
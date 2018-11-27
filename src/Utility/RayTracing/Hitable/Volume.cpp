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

	auto boundaryHitRst = boundary->RayIn(ray);
	if (!boundaryHitRst.hit)
		return HitRst::FALSE;

	auto reverseRay = ToPtr(new Ray(ray->At(Ray::tMin*1.5f), - ray->GetDir()));
	auto reverseHitRst = boundary->RayIn(reverseRay);
	
	HitRst * t0HitRst;
	float t0;
	if (reverseHitRst.hit) {
		// 反向光线撞击到边界, 说明光线在内部, 则此时体积内部的起点为 光线起点
		// 此时以该起点的撞击结果即为前边的 boundaryHitRst
		t0 = 0;
		t0HitRst = &boundaryHitRst;
	}
	else {
		// 否则说明光线在外部, 则此时体积内部的起点为 光线撞击处
		// 此时以该起点的撞击结果需计算
		t0 = boundaryHitRst.record.ray->GetTMax();
		auto t0Ray = ToPtr(new Ray(ray->At(t0), ray->GetDir()));
		t0HitRst = &(boundary->RayIn(t0Ray));

		//太薄
		if (!t0HitRst->hit) {
			ray->SetTMax(originTMax);
			return HitRst::FALSE;
		}
	}

	float t1 = min(originTMax, t0 + t0HitRst->record.ray->GetTMax());
	//此处的 len 未考虑 transform 的 scale
	float lenInVolume = (t1 - t0) * length(ray->GetDir());

	// p = C * dL
	// p(L) = lim(n->inf, (1 - CL/n)^n) = exp(-CL)
	// L = -(1/C)ln(pL)
	float hitLen = -(1.0f / density)*log(Math::Rand_F());

	if (hitLen >= lenInVolume) {
		ray->SetTMax(originTMax);
		return HitRst::FALSE;
	}

	float tFinal = t0 + hitLen / length(ray->GetDir());
	ray->SetTMax(tFinal);

	HitRst hitRst(true);
	hitRst.record = HitRecord(ray, ray->At(tFinal));
	hitRst.material = GetMat();
	hitRst.isMatCoverable = IsMatCoverable();
	return hitRst;
}
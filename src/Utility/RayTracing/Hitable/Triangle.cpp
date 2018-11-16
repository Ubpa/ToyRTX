#include <RayTracing/Triangle.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;


Triangle::Triangle(const Vertex & A, const Vertex & B, const Vertex & C, const Material::CPtr & material)
	: A(A), B(B), C(C), Hitable(material) { }

HitRst Triangle::RayIn(Ray::Ptr & ray) const {
	vec4 abgt = Math::Intersect_RayTri(ray->GetOrigin(), ray->GetDir(), A.pos, B.pos, C.pos);
	if (abgt == vec4(0)
		|| abgt[0] < 0 || abgt[0] > 1
		|| abgt[1] < 0 || abgt[1] > 1
		|| abgt[2] < 0 || abgt[2] > 1
		|| abgt[3] < Ray::tMin || abgt[3] > ray->GetTMax())
		return HitRst::FALSE;

	HitRst hitRst(true);
	hitRst.record.vertex = Vertex::Interpolate_Tri(vec3(abgt[0], abgt[1], abgt[2]), A, B, C);
	//hitRst.hitable = this;
	hitRst.record.ray = ray;
	hitRst.material = material;
	hitRst.isMatCoverable = isMatCoverable;
	ray->SetTMax(abgt[3]);
	return hitRst;
}

AABB Triangle::GetBoundingBox() const {
	vec3 minP = min(min(A.pos, B.pos), C.pos);
	vec3 maxP = max(max(A.pos, B.pos), C.pos);
	for (size_t i = 0; i < 3; i++) {
		if (minP[i] == maxP[i]) {
			minP[i] -= 0.001f;
			maxP[i] += 0.001f;
		}
	}
	return AABB(minP, maxP);
}
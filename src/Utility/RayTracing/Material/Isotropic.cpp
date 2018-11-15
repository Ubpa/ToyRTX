#include <RayTracing/Isotropic.h>
#include <RayTracing/OpTexture.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;


Isotropic::Isotropic(const glm::rgb & color)
	: tex(OpTexture::ConstantTexture(color)) { }

bool Isotropic::Scatter(HitRecord & rec) const {
	auto attenuation = tex->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos);
	rec.ray->Update(rec.vertex.pos, Math::RandInSphere(), attenuation);
	return true;
}
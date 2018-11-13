#include <RayTracing/Lambertian.h>
#include <RayTracing/OpTexture.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Lambertian::Lambertian(const Texture::Ptr & albedo)
	: albedo(albedo) { }

Lambertian::Lambertian(float r, float g, float b)
	: Lambertian(rgb(r, g, b)) { }

Lambertian::Lambertian(const rgb & albedo) {
	this->albedo = OpTexture::ConstantTexture(albedo);
}

bool Lambertian::Scatter(HitRecord & rec) const {
	vec3 dir = rec.normal + Math::RandInSphere();
	rec.ray->Update(rec.pos, dir, albedo->Value(rec.u,rec.v,rec.pos));
	return true;
}
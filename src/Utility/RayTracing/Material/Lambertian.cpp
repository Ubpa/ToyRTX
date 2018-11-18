#include <RayTracing/Lambertian.h>
#include <RayTracing/OpTexture.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Lambertian::Lambertian(const Texture::CPtr & albedo)
	: albedo(albedo) { }

Lambertian::Lambertian(float r, float g, float b)
	: Lambertian(rgb(r, g, b)) { }

Lambertian::Lambertian(const rgb & albedo) {
	this->albedo = OpTexture::ConstantTexture(albedo);
}

bool Lambertian::Scatter(const HitRecord & rec) const {
	vec3 dir = rec.vertex.normal + Math::RandInSphere();
	rec.ray->Update(rec.vertex.pos, dir, albedo->Value(rec.vertex.u,rec.vertex.v,rec.vertex.pos));
	return true;
}
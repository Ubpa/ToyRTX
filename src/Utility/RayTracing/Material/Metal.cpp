#include <RayTracing/OpTexture.h>
#include <RayTracing/Metal.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;


Metal::Metal(const Texture::Ptr & specular, float fuzz)
	: specular(specular), fuzz(fuzz) { }

Metal::Metal(float r, float g, float b, float fuzz)
	: Metal(rgb(r,g,b), fuzz) { }

Metal::Metal(const glm::rgb & specular, float fuzz)
	: fuzz(fuzz) {
	this->specular = OpTexture::ConstantTexture(specular);
}

bool Metal::Scatter(HitRecord & rec) const {
	vec3 dir = reflect(rec.ray->GetDir(), rec.normal);
	vec3 dirFuzz = dir + fuzz * Math::RandInSphere();

	// 反射光线在表面之下
	if (dot(dirFuzz, rec.normal) < 0) {
		rec.ray->SetLightColor(vec3(0));
		return false;
	}

	rec.ray->Update(rec.pos, dirFuzz, specular->Value(rec.u,rec.v,rec.pos));
	return true;
}
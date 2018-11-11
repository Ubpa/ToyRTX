#include <RayTracing/Lambertian.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Lambertian::Lambertian(float r, float g, float b)
	: albedo(vec3(r, g, b)) { }

Lambertian::Lambertian(const vec3 & albedo)
	: albedo(albedo) { }

bool Lambertian::Scatter(HitRecord & rec) const {
	vec3 dir = rec.normal + Math::RandInSphere();
	rec.ray->Update(rec.pos, dir, albedo);

	return true;
}
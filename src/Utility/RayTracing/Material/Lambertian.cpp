#include <RayTracing/Lambertian.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Lambertian::Lambertian(const vec3 & albedo)
	: albedo(albedo) { }

bool Lambertian::Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& rayOut) const {
	vec3 dir = rec.normal + RandInSphere();
	rayOut = Ray(rec.p, dir);
	attenuation = albedo;
	return true;
}
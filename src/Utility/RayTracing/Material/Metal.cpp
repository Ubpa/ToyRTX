#include <RayTracing/Metal.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;

Metal::Metal(const glm::vec3 & specular) : specular(specular) { }

bool Metal::Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& rayOut) const {
	vec3 dir = rayIn.dir - 2 * dot(rayIn.dir, rec.normal) * rec.normal;
	rayOut = Ray(rec.p, dir);
	attenuation = specular;
	return true;
}
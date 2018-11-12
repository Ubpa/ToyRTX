#include <RayTracing/MoveSphere.h>
#include <Utility/Ptr.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;
using namespace std;

MoveSphere::MoveSphere(float t0, float t1, const glm::vec3 & center0, const glm::vec3 & center1, float radius, Material::Ptr material)
	: Sphere(vec3(center0), radius, material), center0(center0), center1(center1), t0(t0), t1(t1) { }

Hitable::HitRst MoveSphere::RayIn(Ray::Ptr & ray) const {
	TRay::Ptr tRay = std::dynamic_pointer_cast<TRay>(ray);
	if(tRay != NULL)
		const_cast<vec3&>(center) = mix(center0, center1, tRay->GetTime());
	return Sphere::RayIn(ray);
}
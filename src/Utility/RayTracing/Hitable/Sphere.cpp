#include <RayTracing/Sphere.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other::Math;
using namespace glm;

Sphere::Sphere(const vec3 & center, float radius, Material::Ptr material)
	: center(center), radius(radius), Hitable(material) { };

Hitable::HitRst Sphere::RayIn(Ray::Ptr & ray) const{
	vec3 oc = ray->GetOrigin() - center;
	float a = dot(ray->GetDir(), ray->GetDir());
	float b = dot(oc, ray->GetDir());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant <= 0)
		return HitRst::FALSE;

	float t = (-b - sqrt(discriminant)) / a;
	if (t > ray->GetTMax() || t < Ray::tMin) {
		t = (-b + sqrt(discriminant)) / a;
		if (t > ray->GetTMax() || t < Ray::tMin)
			return HitRst::FALSE;
	}
	
	ray->SetTMax(t);

	HitRst hitRst(true);
	vec3 pos = ray->At(t);
	vec3 normal = (pos - center) / radius;
	hitRst.record = HitRecord(ray, pos, normal);
	hitRst.hitable = this;

	return hitRst;
}

AABB Sphere::BoundingBox() const {
	vec3 minP = center - vec3(radius);
	vec3 maxP = center + vec3(radius);
	return { minP, maxP };
}
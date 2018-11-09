#include <RayTracing/Sphere.h>

using namespace RayTracing;
using namespace glm;

Sphere::Sphere(const vec3 & center, float radius, Material * material)
	: center(center), radius(radius), material(material) { };

Sphere::~Sphere() { }

bool Sphere::Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
	vec3 oc = ray.origin - center;
	float a = dot(ray.dir, ray.dir);
	float b = dot(oc, ray.dir);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant <= 0)
		return false;

	float t = (-b - sqrt(discriminant)) / a;
	if (t > t_max || t < t_min) {
		t = (-b + sqrt(discriminant)) / a;
		if (t > t_max || t < t_min)
			return false;
	}

	rec.t = t;
	rec.p = ray(t);
	rec.normal = (rec.p - center) / radius;
	return true;
}
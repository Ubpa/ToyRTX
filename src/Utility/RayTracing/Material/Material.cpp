#include <RayTracing/Material.h>

using namespace RayTracing;
using namespace glm;

HitRecord::HitRecord(const Ray::Ptr & ray, const vec3 & pos, const vec3 & normal)
	:ray(ray), pos(pos), normal(normal) { }
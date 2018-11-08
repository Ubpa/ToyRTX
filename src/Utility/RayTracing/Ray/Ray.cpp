#include <RayTracing/Ray.h>

using namespace glm;
using namespace RayTracing;

Ray::Ray(const glm::vec3& origin, const glm::vec3& dir)
	: origin(origin), dir(dir) { }

glm::vec3 Ray::operator ()(float t)const { return  origin + t * dir; }
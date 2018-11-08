#include <RayTracing/TRay.h>

using namespace glm;
using namespace RayTracing;

TRay::TRay(const glm::vec3& origin, const glm::vec3& dir, float time)
	: Ray(origin, dir), time(time) { }
#include <RayTracing/Material.h>

using namespace RayTracing;
using namespace glm;

vec3 Material::Emit(float u, float v, const glm::vec3& p) const {
	return vec3(0);
}
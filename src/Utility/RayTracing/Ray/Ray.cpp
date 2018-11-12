#include <RayTracing/Ray.h>

using namespace glm;
using namespace RayTracing;

Ray::Ray(const vec3& origin, const vec3& dir)
	: origin(origin), dir(dir), color(vec3(1.0f)), tMax(FLT_MAX) { }

vec3 Ray::operator ()(float t) const {
	return  origin + t * dir;
}

vec3 Ray::At(float t) const {
	return origin + t * dir;
}

void Ray::Update(const vec3 & origin, const vec3 & dir, const vec3 & attenuation) {
	this->origin = origin;
	this->dir = dir;
	this->color *= attenuation;
	this->tMax = FLT_MAX;
}


void Ray::SetLightColor(const glm::vec3 & lightColor) {
	this->origin = vec3(0);
	this->dir = vec3(0);
	this->color *= lightColor;
	this->tMax = 0;
}
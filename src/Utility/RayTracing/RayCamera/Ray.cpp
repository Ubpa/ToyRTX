#include <RayTracing/Ray.h>

using namespace glm;
using namespace RayTracing;

const float Ray::tMin = 10e-4;

Ray::Ray(const vec3 & origin, const vec3 & dir){
	Init(origin, dir);
}

vec3 Ray::operator ()(float t) const {
	return  origin + t * dir;
}

vec3 Ray::At(float t) const {
	return origin + t * dir;
}

void Ray::Init(const glm::vec3 & origin, const glm::vec3 & dir) {
	this->origin = origin;
	this->dir = dir;
	this->color = vec3(1.0f);
	this->tMax = FLT_MAX;
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

Ray::Ptr Ray::GenCopy() const {
	auto ray = ToPtr(new Ray);
	CopyTo(ray);
	return ray;
}

void Ray::CopyTo(Ray::Ptr ray) const{
	ray->origin = origin;
	ray->dir = dir;
	ray->color = color;
	ray->tMax = tMax;
}
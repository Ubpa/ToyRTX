#include <RayTracing/Light.h>
#include <RayTracing/ConstTexture.h>

using namespace RayTracing;
using namespace glm;


Light::Light(const rgb & color) {
	lightTex = ToPtr(new ConstTexture(color));
}


bool Light::Scatter(const HitRecord & rec) const {
	rec.ray->SetLightColor(lightTex->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos));
	return false;
}
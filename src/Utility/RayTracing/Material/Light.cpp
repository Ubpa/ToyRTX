#include <RayTracing/Light.h>
#include <RayTracing/OpTexture.h>

using namespace RayTracing;
using namespace glm;


Light::Light(const rgb & color) {
	lightTex = OpTexture::ConstantTexture(color);
}


bool Light::Scatter(HitRecord & rec) const {
	rec.ray->SetLightColor(lightTex->Value(rec.u, rec.v, rec.pos));
	return false;
}
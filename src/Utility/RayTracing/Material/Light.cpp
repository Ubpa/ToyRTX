#include <RayTracing/Light.h>
#include <RayTracing/OpTexture.h>

using namespace RayTracing;
using namespace glm;


Light::Light(const rgb & color) {
	lightTex = OpTexture::ConstantTexture(color);
}


bool Light::Scatter(HitRecord & rec) const {
	rec.ray->SetLightColor(lightTex->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos));
	return false;
}
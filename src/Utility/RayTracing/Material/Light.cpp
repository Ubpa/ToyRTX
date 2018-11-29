#include <RayTracing/Light.h>
#include <RayTracing/ConstTexture.h>

using namespace RayTracing;
using namespace glm;


Light::Light(const rgb & color, float linear, float quadratic)
	: lightTex(ToPtr(new ConstTexture(color))), linear(linear), quadratic(quadratic) { }


bool Light::Scatter(const HitRecord & rec) const {
	float d = rec.ray->GetTMax() * length(rec.ray->GetDir());
	float attDis = 1.0f / (1.0f + d * (linear + quadratic * d));
	float attAngle = abs(dot(normalize(rec.ray->GetDir()), rec.vertex.normal));
	rec.ray->SetLightColor(attDis * attAngle * lightTex->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos));
	return false;
}
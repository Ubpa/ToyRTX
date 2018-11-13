#include <RayTracing/RayTracer.h>

using namespace RayTracing;
using namespace glm;

rgb RayTracer::Trace(Hitable::Ptr & scene, Ray::Ptr & ray, size_t depth) {
	if (depth == 0)
		return rgb(10e-6);

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.hitable->RayOut(hitRst.record))
			return RayTracer::Trace(scene, ray, depth - 1);
		else
			return ray->GetColor();
	}
	else
		return rgb(10e-6);
}
#include <RayTracing/RayTracer.h>

using namespace RayTracing;
using namespace glm;

RayTracer::RayTracer() {
	depth = 0;
}

rgb RayTracer::TraceX(const Hitable::CPtr & scene, Ray::Ptr & ray, size_t depth) {
	if (depth == 0) {
		this->depth += 50-depth;
		return rgb(1.0001f / 255.0f);
	}

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.material == NULL)
			return rgb(1, 0, 1);

		if (hitRst.material->Scatter(hitRst.record))
			return RayTracer::Trace(scene, ray, depth - 1);
		else {
			this->depth += 50 - depth;
			return ray->GetColor();
		}
	}
	else {
		this->depth += 50 - depth;
		return rgb(1.0001f / 255.0f);
	}
}

rgb RayTracer::Trace(const Hitable::CPtr & scene, Ray::Ptr & ray, size_t depth) {
	if (depth == 0)
		return rgb(1.0001f / 255.0f);

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.material == NULL)
			return rgb(1, 0, 1);

		if (hitRst.material->Scatter(hitRst.record))
			return RayTracer::Trace(scene, ray, depth - 1);
		else 
			return ray->GetColor();
	}
	else
		return rgb(1.0001f / 255.0f);
}
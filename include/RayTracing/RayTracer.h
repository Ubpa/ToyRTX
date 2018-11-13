#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include <RayTracing/Hitable.h>
#include <glm/glm.hpp>

namespace RayTracing {
	class RayTracer {
	public:
		static glm::rgb Trace(Hitable::Ptr & scene, Ray::Ptr & ray, size_t depth = 50);
	};
}

#endif // !_RAY_TRACER_H_

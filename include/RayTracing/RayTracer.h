#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include <RayTracing/Hitable.h>
#include <glm/glm.hpp>

namespace RayTracing {
	class RayTracer {
	public:
		RayTracer();

		glm::rgb TraceX(const Hitable::CPtr & scene, Ray::Ptr & ray, size_t depth = 50);

		static glm::rgb Trace(const Hitable::CPtr & scene, Ray::Ptr & ray, size_t depth = 50);

	
		size_t depth;
	};
}

#endif // !_RAY_TRACER_H_

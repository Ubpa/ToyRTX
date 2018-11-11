#ifndef _RAY_CAMERA_H_
#define _RAY_CAMERA_H_

#include <glm/glm.hpp>
#include <RayTracing/Ray.h>

namespace RayTracing {
	class RayCamera : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(RayCamera)
	public:
		// fov is top to bottom in degrees
		RayCamera(const glm::vec3 & pos, const glm::vec3 & viewPoint, float ratioWH, float lenR,
			float fov = 45.0f, float focus_dist = -1.0f, const glm::vec3 & worldUp = glm::vec3(0, 1, 0));

		Ray::Ptr GenRay(float s, float t) const;

	protected:
		glm::vec3 pos;
		glm::vec3 BL_Corner;
		glm::vec3 horizontal;
		glm::vec3 vertical;
		glm::vec3 right, up, front;
		float lenR;
		float t0, t1;

		static const float PI;
	};
}

#endif // !_RAY_CAMERA_H_

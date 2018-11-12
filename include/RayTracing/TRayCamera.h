#ifndef _T_RAY_CAMERA_H_
#define _T_RAY_CAMERA_H_

#include <glm/glm.hpp>
#include <RayTracing/TRay.h>
#include <RayTracing/RayCamera.h>

namespace RayTracing {
	class TRayCamera : public RayCamera{
	   HEAP_OBJ_SETUP(TRayCamera)
	public:
		TRayCamera(const glm::vec3 & pos, const glm::vec3 & viewPoint, float ratioWH,
			float t0 = 0.0f, float t1 = 0.0f,
			float fov = 45.0f, float lenR = 0.0f, float focus_dist = -1.0f,
			const glm::vec3 & worldUp = glm::vec3(0, 1, 0));

		virtual Ray::Ptr GenRay(float s, float t) const;
	private:

		glm::vec3 pos;
		glm::vec3 BL_Corner;
		glm::vec3 horizontal;
		glm::vec3 vertical;
		glm::vec3 right, up, front;
		float lenR;
		float t0, t1;
	};
}

#endif // !_RAY_CAMERA_H_
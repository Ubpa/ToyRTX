#ifndef _T_RAY_H_
#define _T_RAY_H_

#include <RayTracing/Ray.h>

namespace RayTracing {
	class TRay : public Ray {
		HEAP_OBJ_SETUP(TRay)
	public:
		typedef CppUtility::Other::Ptr<TRay> Ptr;

		TRay(const glm::vec3& origin, const glm::vec3& dir, float time = 0.0f);
		float time;
	};
}
#endif //!_T_RAY_H_
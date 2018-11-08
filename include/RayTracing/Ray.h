#ifndef _RAY_H_
#define _RAY_H_

#include <glm/glm.hpp>

namespace RayTracing {
	class Ray {
	public:
		Ray(const glm::vec3& origin, const glm::vec3& dir);

		glm::vec3 operator()(float t)const;

		glm::vec3 origin;
		glm::vec3 dir;
	};
}
#endif //!_RAY_H_
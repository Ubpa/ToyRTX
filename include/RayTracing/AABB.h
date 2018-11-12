#ifndef _AABB_H_
#define _AABB_H_

#include <RayTracing/Ray.h>
#include <glm/glm.hpp>

namespace RayTracing {
	class AABB {
	public:
		AABB(const glm::vec3 & minP, const glm::vec3 & maxP, bool isValid = true);
		glm::vec3 GetMinP() const { return minP; }
		glm::vec3 GetMaxP() const { return maxP; }
		
		bool Hit(const Ray::Ptr & ray) const;

		AABB operator +(const AABB & aabb) const;
		AABB & operator +=(const AABB & aabb);

		static const AABB InValid;
	private:
		bool isValid;
		glm::vec3 minP;
		glm::vec3 maxP;
	};
}

#endif // !_AABB_H_


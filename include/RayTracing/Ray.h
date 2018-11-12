#ifndef _RAY_H_
#define _RAY_H_

#include <Utility/HeapObj.h>
#include <glm/glm.hpp>

namespace RayTracing {
	class Ray : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Ray)
	public:
		Ray(const glm::vec3& origin, const glm::vec3& dir);

		glm::vec3 operator()(float t) const;
		glm::vec3 At(float t) const;

		glm::vec3 GetOrigin() const { return origin; };
		glm::vec3 GetDir() const { return dir; }
		glm::vec3 GetColor() const { return color; }
		float GetTMax() const { return tMax; }
		void SetTMax(float tMax) { this->tMax = tMax; }

		void Update(const glm::vec3 & origin, const glm::vec3 & dir, const glm::vec3 & attenuation);
		void SetLightColor(const glm::vec3 & lightColor);

	protected:

		glm::vec3 origin;
		glm::vec3 dir;
		glm::vec3 color;
		float tMax;
	};
}
#endif //!_RAY_H_
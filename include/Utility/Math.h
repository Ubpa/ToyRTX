#ifndef _MATH_H_
#define _MATH_H_

#include <glm/glm.hpp>

namespace CppUtility {
	namespace Other {
		glm::vec2 RandInCircle();
		glm::vec3 RandInSphere();
		int Rand_I();
		unsigned int Rand_UI();
		float Rand_F();
		double Rand_D();
	}
}

#endif // !_MATH_H_

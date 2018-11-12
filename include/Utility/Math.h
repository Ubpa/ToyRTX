#ifndef _MATH_H_
#define _MATH_H_

#include <glm/glm.hpp>
#include <vector>

namespace CppUtility {
	namespace Other {
		typedef glm::vec3 rgb;
		namespace Math {
			const float EPSILON = 10e-6;

			glm::vec2 RandInCircle();

			glm::vec3 RandInSphere();

			//-0x7FFFFFFFF -- 0x7FFFFFFF
			int Rand_I();

			//0 -- 0xFFFFFFFF
			unsigned int Rand_UI();

			//0.0f -- 1.0f
			float Rand_F();

			//0.0 -- 1.0
			double Rand_D();

			float Gray(const glm::vec3 & color);

			// 折射
			// rationNiNt 折射率之比 == Ni / Nt
			//     Ni 为 入射光线所在介质的折射率
			//     Nt 为 折射光线所在介质的折射率
			bool Refract(const glm::vec3 & viewDir, const glm::vec3 & normal, float ratioNiNt, glm::vec3 & refractDir);

			// 反射率
			// viewDir 为 视线方向(从物点到视点的方向)
			// halfway 为 视线方向与光线方向的平均
			// rationNtNi 折射率之比 == Nt / Ni
			//     Ni 为 入射光线所在介质的折射率
			//     Nt 为 折射光线所在介质的折射率
			float FresnelSchlick(const glm::vec3 & viewDir, const glm::vec3 & halfway, float ratioNtNi);

			template <typename T>
			T Mean(const std::vector<T> & data) {
				if (data.size() == 0)
					return static_cast<T>(0);

				T sum = static_cast<T>(0);
				for (size_t i = 0; i < data.size(); i++)
					sum += data[i];

				return sum / data.size();
			}

			template<typename T>
			T Variance(const std::vector<T> & data) {
				if (data.size() <= 1)
					return static_cast<T>(0);

				T mean = Mean(data);
				T sum = static_cast<T>(0);
				for (size_t i = 0; i < data.size(); i++)
					sum += pow(data[i]-mean, 2);

				return sum / (data.size()-1);
			}
		}
	}
}

#endif // !_MATH_H_

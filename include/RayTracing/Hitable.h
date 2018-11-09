#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <RayTracing/Ray.h>
#include <Utility/Ptr.h>

namespace RayTracing {
	class Material;

	struct HitRecord {
		float t;
		glm::vec3 p;
		glm::vec3 normal;
		Material * material;
	};

	class Hitable {
	public:
		template<typename T>
		static CppUtility::Other::Ptr<T> ToPtr(T * op);
		virtual bool Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const = 0;
	protected:
		static void ProtectedDelete(Hitable * op);
		virtual ~Hitable();
	};

	template<typename T>
	static CppUtility::Other::Ptr<T> Hitable::ToPtr(T * hitable) {
		return CppUtility::Other::Ptr<T>(hitable, ProtectedDelete);
	}
}
#endif
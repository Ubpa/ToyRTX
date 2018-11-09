#ifndef _GROUP_H_
#define _GROUP_H_

#include <RayTracing/Hitable.h>
#include <vector>

namespace RayTracing {
	class Group : public Hitable, public std::vector<CppUtility::Other::Ptr<Hitable>>{
	public:
		Group();
		virtual bool Hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		Group & operator <<(const CppUtility::Other::Ptr<Hitable> & hitable);
	protected:
		virtual ~Group();
	};
}
#endif //!_GROUP_H_


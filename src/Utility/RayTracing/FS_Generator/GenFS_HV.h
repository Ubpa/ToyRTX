#ifndef _GEN_FS_HV_H_
#define _GEN_FS_HV_H_

#include <RayTracing/HitableVisitor.h>

#include <vector>
#include <map>

namespace RayTracing {
	class Material;

	class GenFS_HV : private HitableVisitor{
		HEAP_OBJ_SETUP(GenFS_HV)
	public:
		virtual void Visit(const Group * group);
	private:
		virtual void Visit(const Sphere * sphere);
		virtual void Visit(const Sky * sky);

		std::vector<float> sceneData;
		std::map<CppUtility::Other::CPtr<Material>, size_t> mat2idx;
	};
}

#endif // !_GEN_FS_HV_H_

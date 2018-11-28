#ifndef _GEN_FS_HV_H_
#define _GEN_FS_HV_H_

#include <RayTracing/HitableVisitor.h>

#include <Utility/Ptr.h>

#include <vector>
#include <map>

namespace RayTracing {
	class Material;
	class GenData_MV;

	// Generate Fragmen Shader -- Hitable Visitor
	class GenData_HV : public HitableVisitor{
		HEAP_OBJ_SETUP(GenData_HV)
	public:
		typedef std::map<CppUtility::Other::CPtr<Material>, size_t> MatIdxMap;

		const std::vector<float> & GetSceneData() const { return sceneData; };
		void Accept(const CppUtility::Other::Ptr<GenData_MV> & genFS_MV);
	private:
		void SetMat(const MatIdxMap & mat2idx);
		virtual void Visit(const CppUtility::Other::CPtr<Hitable> & hitable);
		virtual void Visit(const CppUtility::Other::CPtr<Group> & group);
		virtual void Visit(const CppUtility::Other::CPtr<Sphere> & sphere);
		virtual void Visit(const CppUtility::Other::CPtr<BVH_Node> & bvhNode);
		using HitableVisitor::Visit;

		std::vector<float> sceneData;
		std::map< CppUtility::Other::CPtr<Hitable>, size_t> hitable2idx;
		std::map<CppUtility::Other::CPtr<Material>, std::vector<size_t>> mat2idxVec;
	};
}

#endif // !_GEN_FS_HV_H_

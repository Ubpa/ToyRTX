#ifndef _GEN_FS_HV_H_
#define _GEN_FS_HV_H_

#include <RayTracing/HitableVisitor.h>

#include <Utility/Ptr.h>

#include <vector>
#include <map>

namespace RayTracing {
	class Material;
	class MatVisitor;

	// Generate Fragmen Shader -- Hitable Visitor
	class GenFS_HV : public HitableVisitor{
	public:
		typedef std::map<const Material *, size_t> MatIdxMap;

		void SetMat(const MatIdxMap & mat2idx);
		const std::vector<float> & GetSceneData() const { return sceneData; };
		void Accept(MatVisitor * matVisitor);
	private:
		virtual void Visit(const Group * group);
		virtual void Visit(const Sphere * sphere);
		using HitableVisitor::Visit;

		std::vector<float> sceneData;
		std::map<CppUtility::Other::CPtr<Material>, std::vector<size_t>> mat2idxVec;
	};
}

#endif // !_GEN_FS_HV_H_

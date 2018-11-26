#ifndef _GEN_FS_MV_H_
#define _GEN_FS_MV_H_

#include <RayTracing/MatVisitor.h>

#include <Utility/Ptr.h>

#include <vector>
#include <map>

namespace RayTracing {
	class Material;
	class Texture;
	class TexVisitor;

	// Generate Fragmen Shader -- Material Visitor
	class GenFS_MV : public MatVisitor{
	public:
		typedef std::map<const Material *, size_t> MatIdxMap;
		typedef std::map<const Texture *, size_t> TexIdxMap;

		void SetTex(const TexIdxMap & tex2idx);
		const MatIdxMap & GetMatIdxMap() const { return mat2idx; }
		const std::vector<float> & GetMatData() const { return matData; };
		void Accept(TexVisitor * texVisitor);
	private:
		virtual void Visit(const Lambertian * lambertian);
		virtual void Visit(const Metal * metal);
		virtual void Visit(const Dielectric * dielectric);
		using MatVisitor::Visit;

		std::vector<float> matData;
		std::map<CppUtility::Other::CPtr<Texture>, std::vector<size_t>> tex2idxVec;
		MatIdxMap mat2idx;
	};
}

#endif // !_GEN_FS_HV_H_

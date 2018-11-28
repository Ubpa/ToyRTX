#ifndef _GEN_FS_TV_H_
#define _GEN_FS_TV_H_

#include <RayTracing/TexVisitor.h>

#include <vector>
#include <map>

namespace RayTracing {
	class Texture;

	// Generate Fragmen Shader -- Texture Visitor
	class GenData_TV : public TexVisitor{
		HEAP_OBJ_SETUP(GenData_TV)
	public:
		typedef std::map<CppUtility::Other::CPtr<Texture>, size_t> TexIdxMap;

		const TexIdxMap & GetTexIdxMap() const { return tex2idx; }
		const std::vector<float> & GetTexData() const { return texData; };
	private:
		virtual void Visit(const CppUtility::Other::CPtr<ConstTexture> & constTexture);
		using TexVisitor::Visit;

		std::vector<float> texData;
		TexIdxMap tex2idx;
	};
}

#endif // !_GEN_FS_TV_H_

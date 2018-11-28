#ifndef _GEN_FS_TV_H_
#define _GEN_FS_TV_H_

#include <RayTracing/TexVisitor.h>

#include <vector>
#include <map>

namespace CppUtility {
	namespace Other {
		class Image;
	}
}

namespace RayTracing {
	// Generate Fragmen Shader -- Texture Visitor
	class GenData_TV : public TexVisitor{
		HEAP_OBJ_SETUP(GenData_TV)
	public:
		typedef std::map<CppUtility::Other::CPtr<Texture>, size_t> TexIdxMap;
		typedef std::map<CppUtility::Other::CPtr<CppUtility::Other::Image>, size_t> ImgIdxMap;

		const TexIdxMap & GetTexIdxMap() const { return tex2idx; }
		const ImgIdxMap & GetImgIdxMap() const { return img2idx; }
		const std::vector<float> & GetTexData() const { return texData; };
	private:
		virtual void Visit(const CppUtility::Other::CPtr<ConstTexture> & constTexture);
		virtual void Visit(const CppUtility::Other::CPtr<ImgTexture> & imgTexture);
		using TexVisitor::Visit;

		std::vector<float> texData;
		TexIdxMap tex2idx;
		ImgIdxMap img2idx;
	};
}

#endif // !_GEN_FS_TV_H_

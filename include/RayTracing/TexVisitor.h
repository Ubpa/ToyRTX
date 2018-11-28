#ifndef _TEX_VISITOR_H_
#define _TEX_VISITOR_H_

#include <Utility/HeapObj.h>

namespace RayTracing {
	class Texture;
	class ConstTexture;
	class OpTexture;
	class ImgTexture;

	class TexVisitor : public CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(TexVisitor)
	public:
		virtual void Visit(const CppUtility::Other::CPtr<Texture> & tex);
		virtual void Visit(const CppUtility::Other::CPtr<ConstTexture> & constTexture);
		virtual void Visit(const CppUtility::Other::CPtr<OpTexture> & opTexture);
		virtual void Visit(const CppUtility::Other::CPtr<ImgTexture> & imgTexture);
	private:
	};
}

#endif // !_TEX_VISITOR_H_

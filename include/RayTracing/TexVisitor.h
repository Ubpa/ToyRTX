#ifndef _TEX_VISITOR_H_
#define _TEX_VISITOR_H_

#include <Utility/HeapObj.h>

namespace RayTracing {
	class ConstTexture;
	class OpTexture;
	class ImgTexture;

	class TexVisitor : CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(TexVisitor)
	public:
		virtual void Visit(const ConstTexture * constTexture);
		virtual void Visit(const OpTexture * opTexture);
		virtual void Visit(const ImgTexture * imgTexture);
	private:
	};
}

#endif // !_TEX_VISITOR_H_

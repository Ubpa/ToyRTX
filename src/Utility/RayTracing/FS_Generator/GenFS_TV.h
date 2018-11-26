#ifndef _GEN_FS_TV_H_
#define _GEN_FS_TV_H_

#include <RayTracing/TexVisitor.h>
#include <string>

namespace RayTracing {
	class GenFS_TV : private TexVisitor{
		HEAP_OBJ_SETUP(GenFS_TV)
	};
}

#endif // !_GEN_FS_TV_H_

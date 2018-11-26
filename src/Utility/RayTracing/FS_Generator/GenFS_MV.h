#ifndef _GEN_FS_MV_H_
#define _GEN_FS_MV_H_

#include <RayTracing/MatVisitor.h>
#include <string>

namespace RayTracing {
	class GenFS_MV : private MatVisitor{
		HEAP_OBJ_SETUP(GenFS_MV)
	};
}

#endif // !_GEN_FS_HV_H_

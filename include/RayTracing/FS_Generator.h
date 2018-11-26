#ifndef _FS_GENERATOR_H_
#define _FS_GENERATOR_H_

#include <vector>
#include <string>

#include "GenFS_HV.h"
#include "GenFS_MV.h"
#include "GenFS_TV.h"

namespace RayTracing {
	class FS_Generator {
	public:
		FS_Generator();
	private:
		const GenFS_HV::Ptr hitableVisitor;
		const GenFS_MV::Ptr matVisitor;
		const GenFS_TV::Ptr texVisitor;
	};
}

#endif // !_SHADER_GENERATOR_H_

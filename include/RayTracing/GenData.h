#ifndef _FS_GENERATOR_H_
#define _FS_GENERATOR_H_

#include <Utility/Ptr.h>
#include <vector>
#include <string>
#include <sstream>

namespace RayTracing {
	class Hitable;
	class Group;
	class GenData_HV;
	class GenData_MV;
	class GenData_TV;

	class GenData {
	public:
		GenData(const CppUtility::Other::CPtr<Hitable> & scene);
		
		const std::vector<float> GetSceneData() const;
		const std::vector<float> GetMatData() const;
		const std::vector<float> GetTexData() const;

	private:

		CppUtility::Other::Ptr<GenData_HV> hitableVisitor;
		CppUtility::Other::Ptr<GenData_MV> matVisitor;
		CppUtility::Other::Ptr<GenData_TV> texVisitor;

		CppUtility::Other::CPtr<Group> root;
	};
}

#endif // !_SHADER_GENERATOR_H_

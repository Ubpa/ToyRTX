#ifndef _FS_GENERATOR_H_
#define _FS_GENERATOR_H_

#include <Utility/Ptr.h>
#include <vector>
#include <string>
#include <sstream>

namespace RayTracing {
	class Hitable;
	class Group;
	class GenFS_HV;
	class GenFS_MV;
	class GenFS_TV;

	class FS_Generator {
	public:
		FS_Generator(const CppUtility::Other::CPtr<Hitable> & scene);
		const std::string BuildFS();
		
		const std::vector<float> GetSceneData() const;
		const std::vector<float> GetMatData() const;
		const std::vector<float> GetTexData() const;

	private:
		//void Data(std::stringstream & shaderSS);
		void Uniform(std::stringstream & shaderSS);

		static void Version(std::stringstream & shaderSS);
		static void LayOut(std::stringstream & shaderSS);
		static void Struct(std::stringstream & shaderSS);
		//static void Uniform(std::stringstream & shaderSS);
		static void Variable(std::stringstream & shaderSS);
		static void FuncDefine(std::stringstream & shaderSS);
		static void Main(std::stringstream & shaderSS);
		static void RandFunc(std::stringstream & shaderSS);
		static void MathFunc(std::stringstream & shaderSS);
		static void Scatter(std::stringstream & shaderSS);
		static void RayIn(std::stringstream & shaderSS);
		static void Value(std::stringstream & shaderSS);
		static void RayFunc(std::stringstream & shaderSS);

		CppUtility::Other::Ptr<GenFS_HV> hitableVisitor;
		CppUtility::Other::Ptr<GenFS_MV> matVisitor;
		CppUtility::Other::Ptr<GenFS_TV> texVisitor;

		CppUtility::Other::CPtr<Group> root;
	};
}

#endif // !_SHADER_GENERATOR_H_

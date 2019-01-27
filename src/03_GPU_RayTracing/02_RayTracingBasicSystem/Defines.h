#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <glm/glm.hpp>
#include <vector>

namespace Define {
	const std::string str_Chapter = "03_GPU_RayTracing";
	const std::string str_Subchapter = "02_RTXBasicSystem";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;

	const std::string str_ShaderPrefix = "/data/shaders/" + str_Chapter + "/" + str_Subchapter + "/";

	const std::string str_RTXBasicSystem = "RTXBasicSystem";
	const std::string str_RTXBasicSystem_vs = str_Basic_P2T2_vs;
	const std::string str_RTXBasicSystem_fs = str_ShaderPrefix + str_RTXBasicSystem + str_FsPostfix;
}

#endif // !_DEFINES_H_



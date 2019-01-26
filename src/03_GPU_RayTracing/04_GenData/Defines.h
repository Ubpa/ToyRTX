#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <Utility/OGL/CommonDefine.h>
#include <glm/glm.hpp>
#include <vector>

namespace Define {
	const std::string str_Chapter = "03_GPU_RayTracing";
	const std::string str_Subchapter = "04_GenData";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;

	const std::string str_ShaderPrefix = "/data/shaders/" + str_Chapter + "/" + str_Subchapter + "/";

	const std::string str_RTX = "RTX";
	const std::string str_RTX_vs = str_Basic_P2T2_vs;
	const std::string str_RTX_fs = str_ShaderPrefix + str_RTX + str_FsPostfix;
}

#endif // !_DEFINES_H_



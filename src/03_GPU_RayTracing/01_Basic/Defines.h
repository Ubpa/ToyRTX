#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <OpenGL/CommonDefine.h>

namespace Define {
	const std::string str_Chapter = "03_GPU_RayTracing";
	const std::string str_Subchapter = "01_Basic";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;

	const std::string str_ShaderPrefix = "/data/shaders/" + str_Chapter + "/" + str_Subchapter + "/";

	const std::string str_RayTracingBasic = "rayTracingBasic";
	const std::string str_RayTracingBasic_vs = str_Basic_P2T2_vs;
	const std::string str_RayTracingBasic_fs = str_ShaderPrefix + str_RayTracingBasic + str_FsPostfix;

}

#endif // !_DEFINES_H_



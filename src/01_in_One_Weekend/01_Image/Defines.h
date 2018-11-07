#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <OpenGL/CommonDefine.h>
#include <Utility/Config.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Define {
	const std::string str_Chapter = "01_in_a_Weekend";
	const std::string str_Subchapter = "01_Image";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;


	//------------ shader
	const std::string str_ShaderPrefix = "/data/shaders/" + str_Chapter + "/" + str_Subchapter + "/";

	const std::string str_PostProcess = "postProcess";
	const std::string str_PostProcess_vs = str_Basic_P2T2_vs;
	const std::string str_PostProcess_fs = str_ShaderPrefix + str_PostProcess + str_FsPostfix;

	//------------ value
	const size_t val_ImgWidth = 200;
	const size_t val_ImgHeight = 100;
	const size_t val_ImgChannel = 3;

	CppUtility::Other::Config * DoConfig();
	float lerp(float a, float b, float f);
}

#endif // !_DEFINES_H_



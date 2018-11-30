#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <OpenGL/CommonDefine.h>
#include <glm/glm.hpp>
#include <vector>

namespace Define {
	const std::string str_Chapter = "03_GPU_RayTracing";
	const std::string str_Subchapter = "11_All";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;

	const std::string str_Img_Earth = "/data/textures/earth.jpg";

	const std::string str_ShaderPrefix = "/data/shaders/" + str_Chapter + "/" + str_Subchapter + "/";

	const std::string str_RTX = "RTX";
	const std::string str_RTX_vs = str_Basic_P2T2_vs;
	const std::string str_RTX_fs = str_ShaderPrefix + str_RTX + str_FsPostfix;

	const float data_SquareVertexPos[36] = {
		-0.5, -0.5,  0,  0,  0,  1,
		 0.5, -0.5,  0,  0,  0,  1,
		-0.5,  0.5,  0,  0,  0,  1,
		 0.5,  0.5,  0,  0,  0,  1,
		-0.5,  0.5,  0,  0,  0,  1,
		 0.5, -0.5,  0,  0,  0,  1,
	};
	
	const float data_CubeVertexPos[216] = {
		//up
		-0.5,  0.5,  0.5,  0,  1,  0,
		 0.5,  0.5,  0.5,  0,  1,  0,
		-0.5,  0.5, -0.5,  0,  1,  0,
		 0.5,  0.5, -0.5,  0,  1,  0,
		-0.5,  0.5, -0.5,  0,  1,  0,
		 0.5,  0.5,  0.5,  0,  1,  0,
		 //down
		-0.5, -0.5,  0.5,  0, -1,  0,
		 0.5, -0.5,  0.5,  0, -1,  0,
		-0.5, -0.5, -0.5,  0, -1,  0,
		 0.5, -0.5, -0.5,  0, -1,  0,
		-0.5, -0.5, -0.5,  0, -1,  0,
		 0.5, -0.5,  0.5,  0, -1,  0,
		 //left
		-0.5, -0.5,  0.5, -1,  0,  0,
		-0.5, -0.5, -0.5, -1,  0,  0,
		-0.5,  0.5,  0.5, -1,  0,  0,
		-0.5,  0.5, -0.5, -1,  0,  0,
		-0.5,  0.5,  0.5, -1,  0,  0,
		-0.5, -0.5, -0.5, -1,  0,  0,
		//right
		0.5, -0.5,  0.5,  1,  0,  0,
		0.5, -0.5, -0.5,  1,  0,  0,
		0.5,  0.5,  0.5,  1,  0,  0,
		0.5,  0.5, -0.5,  1,  0,  0,
		0.5,  0.5,  0.5,  1,  0,  0,
		0.5, -0.5, -0.5,  1,  0,  0,
		//front
	   -0.5, -0.5,  0.5,  0,  0,  1,
		0.5, -0.5,  0.5,  0,  0,  1,
	   -0.5,  0.5,  0.5,  0,  0,  1,
		0.5,  0.5,  0.5,  0,  0,  1,
	   -0.5,  0.5,  0.5,  0,  0,  1,
		0.5, -0.5,  0.5,  0,  0,  1,
		//back
	   -0.5, -0.5, -0.5,  0,  0, -1,
		0.5, -0.5, -0.5,  0,  0, -1,
	   -0.5,  0.5, -0.5,  0,  0, -1,
		0.5,  0.5, -0.5,  0,  0, -1,
	   -0.5,  0.5, -0.5,  0,  0, -1,
		0.5, -0.5, -0.5,  0,  0, -1,
	};
}

#endif // !_DEFINES_H_



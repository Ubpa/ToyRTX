#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <glm/glm.hpp>
#include <vector>

namespace Define {
	const std::string str_Chapter = "04_Extension";
	const std::string str_Subchapter = "03_GPU_Skybox_Model";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;

	const std::string str_ShaderPrefix = "/data/shaders/" + str_Chapter + "/" + str_Subchapter + "/";

	const std::string str_RTX = "RTX";
	const std::string str_RTX_vs = str_Basic_P2T2_vs;
	const std::string str_RTX_fs = str_ShaderPrefix + str_RTX + str_FsPostfix;

	const std::string str_Img_Prefix = "/data/textures/";

	const std::string str_Img_Earth = str_Img_Prefix + "earth.jpg";
	const std::string str_Img_Wood = str_Img_Prefix + "wood.jpg";
	const std::string str_Img_SkySphere = str_Img_Prefix + "skySphere.png";

	const std::vector<std::string> str_Vec_Img_Skybox = {
		str_Img_Prefix + "skybox/right.jpg",
		str_Img_Prefix + "skybox/left.jpg",
		str_Img_Prefix + "skybox/top.jpg",
		str_Img_Prefix + "skybox/bottom.jpg",
		str_Img_Prefix + "skybox/front.jpg",
		str_Img_Prefix + "skybox/back.jpg"
	};

	const std::vector<std::string> str_Vec_Img_Skybox2 = {
		str_Img_Prefix + "skybox2/riverside_east.BMP",
		str_Img_Prefix + "skybox2/riverside_west.BMP",
		str_Img_Prefix + "skybox2/riverside_up.BMP",
		str_Img_Prefix + "skybox2/riverside_down.BMP",
		str_Img_Prefix + "skybox2/riverside_north.BMP",
		str_Img_Prefix + "skybox2/riverside_south.BMP"
	};

	const std::string str_Object_Prefix = "/data/objects/";

	const std::string str_Obj_Bunny = str_Object_Prefix+"bunny/bunny.obj";
	const std::string str_Obj_Dragon = str_Object_Prefix + "dragon/dragon.obj";

	const float data_SquareVertexPos[48] = {
		-0.5, -0.5,  0,  0,  0,  1, 0, 0,
		 0.5, -0.5,  0,  0,  0,  1, 1, 0,
		-0.5,  0.5,  0,  0,  0,  1, 0, 1,
		 0.5,  0.5,  0,  0,  0,  1, 1, 1,
		-0.5,  0.5,  0,  0,  0,  1, 0, 1,
		 0.5, -0.5,  0,  0,  0,  1, 1, 0,
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



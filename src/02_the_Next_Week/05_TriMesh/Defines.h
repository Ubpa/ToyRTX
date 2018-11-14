#ifndef _DEFINES_H_
#define _DEFINES_H_

namespace Define {
	const std::string str_Chapter = "02_the_Next_Week";
	const std::string str_Subchapter = "04_RectLight";
	const std::string str_WindowTitle = str_Chapter + "_" + str_Subchapter;

	const std::string str_Img_Earth = "/data/textures/earth.jpg";

	const float data_RectVertexPos[18] = {
		2, 0.5f, -1,
		2, 0.5f,  1,
		2, 1.5f, -1,
		2, 1.5f,  1,
		2, 1.5f, -1,
		2, 0.5f,  1,
	};
}

#endif // !_DEFINES_H_



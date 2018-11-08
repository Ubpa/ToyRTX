#ifndef _IMG_WINDOW_H_
#define _IMG_WINDOW_H_

#include <Utility/Operation.h>
#include <Utility/Image.h>

namespace RayTracing {
	class ImgWindow {
	public:
		enum ENUM_OPTION {
			ENUM_OPTION_EMPTY = 0,
			ENUM_OPTION_POST_PROCESS_FLIP = 1 << 0,
			ENUM_OPTION_POST_PROCESS_BLUR = 1 << 1,
			ENUM_OPTION_SAVE_SRC_IMG = 1 << 2,
			ENUM_OPTION_SAVE_POST_PROCESS_IMG = 1 << 3,
			ENUM_OPTION_POST_PROCESS_ALL = ENUM_OPTION_POST_PROCESS_FLIP | ENUM_OPTION_POST_PROCESS_BLUR,
			ENUM_OPTION_BASIC = ENUM_OPTION_POST_PROCESS_FLIP | ENUM_OPTION_SAVE_SRC_IMG,
			ENUM_OPTION_SAVE_ALL_IMG = ENUM_OPTION_SAVE_SRC_IMG | ENUM_OPTION_SAVE_POST_PROCESS_IMG,
			ENUM_OPTION_ALL = ENUM_OPTION_POST_PROCESS_ALL | ENUM_OPTION_SAVE_ALL_IMG,
		};
		ImgWindow(const std::string & title = "Image_Window", size_t fps = 60, ENUM_OPTION option = ENUM_OPTION_EMPTY);
		bool Run(const CppUtility::Other::Ptr<CppUtility::Other::Operation> & imgUpdateOp);
		double GetScale() const { return scale; };
		CppUtility::Other::Image & GetImg() { return img; };
		bool IsValid() { return isValid; };
	private:
		const size_t fps;
		const std::string title;
		double scale;
		const ENUM_OPTION option;
		std::string rootPath;
		bool isValid;
		CppUtility::Other::Image img;
	};
}

#endif // !_IMG_WINDOW_H_

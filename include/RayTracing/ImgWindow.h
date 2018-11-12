#ifndef _IMG_WINDOW_H_
#define _IMG_WINDOW_H_

#include <Utility/Config.h>
#include <Utility/Operation.h>
#include <Utility/Image.h>

namespace RayTracing {
	class ImgWindow {
	public:
		enum ENUM_OPTION {
			ENUM_OPTION_EMPTY = 0,
			ENUM_OPTION_POST_PROCESS_FLIP = 1 << 0,
			ENUM_OPTION_POST_PROCESS_BLUR = 1 << 1,
			ENUM_OPTION_POST_PROCESS_INTERPOLATION = 1 << 2,
			ENUM_OPTION_SAVE_SRC_IMG = 1 << 3,
			ENUM_OPTION_SAVE_POST_PROCESS_IMG = 1 << 4,
			ENUM_OPTION_POST_PROCESS_ALL = ENUM_OPTION_POST_PROCESS_FLIP | ENUM_OPTION_POST_PROCESS_BLUR | ENUM_OPTION_POST_PROCESS_INTERPOLATION,
			ENUM_OPTION_BASIC = ENUM_OPTION_POST_PROCESS_FLIP | ENUM_OPTION_POST_PROCESS_INTERPOLATION | ENUM_OPTION_SAVE_SRC_IMG,
			ENUM_OPTION_SAVE_ALL_IMG = ENUM_OPTION_SAVE_SRC_IMG | ENUM_OPTION_SAVE_POST_PROCESS_IMG,
			ENUM_OPTION_ALL = ENUM_OPTION_POST_PROCESS_ALL | ENUM_OPTION_SAVE_ALL_IMG,
		};

		ImgWindow(const std::string & title = "Image_Window");

		double GetScale() const { return scale; };
		CppUtility::Other::Image & GetImg() { return img; };
		bool IsValid() { return isValid; };

		bool Run(const CppUtility::Other::Ptr<CppUtility::Other::Operation> & imgUpdateOp);
	private:
		CppUtility::Other::Ptr<CppUtility::Other::Config> DoConfig();

		size_t fps;
		const std::string title;
		double scale;
		ENUM_OPTION option;
		std::string rootPath;
		bool isValid;
		CppUtility::Other::Image img;
		size_t blurNum;
	};
}

#endif // !_IMG_WINDOW_H_

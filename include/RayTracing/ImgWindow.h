#ifndef _IMG_WINDOW_H_
#define _IMG_WINDOW_H_

#include <RayTracing/TexWindow.h>

namespace RayTracing {
	class ImgWindow : public TexWindow{
	public:
		ImgWindow(const std::string & title = "Image_Window");

		CppUtility::Other::Image & GetImg() { return img; };

		bool Run(const CppUtility::Other::Operation::Ptr & imgUpdateOp);
	private:
		CppUtility::Other::Image img;
	};
}

#endif // !_IMG_WINDOW_H_

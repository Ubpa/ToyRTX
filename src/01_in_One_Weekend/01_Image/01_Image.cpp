#include <RayTracing/ImgWindow.h>

#include <Utility/Image.h>
#include <Utility/LambdaOp.h>

#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace std;

int main(int argc, char ** argv) {
	ImgWindow imgWindow(str_WindowTitle);
	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();
	const size_t val_ImgChannel = img.GetChannel();

	auto imgUpdate = Operation::ToPtr(new LambdaOp([&]() {
		for (size_t i = 0; i < img.GetWidth(); i++) {
			for (size_t j = 0; j < img.GetHeight(); j++) {
				float r = i / (float)img.GetWidth();
				float g = j / (float)img.GetHeight();
				float b = 0.2;
				img.SetPixel(i, j, Image::Pixel<float>(r, g, b));
			}
		}
	}, false));

	imgWindow.Run(imgUpdate);

	return 0;
}


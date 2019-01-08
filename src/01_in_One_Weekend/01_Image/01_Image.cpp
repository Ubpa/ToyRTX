#include <RayTracing/ImgWindow.h>

#include <Utility/Image.h>
#include <Utility/LambdaOp.h>
#include <Utility/Math.h>
#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace std;

int main(int argc, char ** argv) {
	Image img(400, 400, 4);
	for (size_t i = 0; i < 400; i++) {
		for (size_t j = 0; j < 400; j++) {
			if((i+j)%2==0)
				img.SetPixel(i, j, Image::Pixel<float>(0, 0, 0, Math::Rand_F()));
			else {
				img.SetPixel(i, j, Image::Pixel<float>(10.0+i/400.0, 10.0 + i/400.0, 0, 0.1));
			}
		}
	}
	img.SaveAsPNG("test.png");
	return 0;
	/*

	ImgWindow imgWindow(str_WindowTitle);
	if (!imgWindow.IsValid()) {
		printf("ERROR: Image Window Create Fail.\n");
		return 1;
	}

	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();
	const size_t val_ImgChannel = img.GetChannel();

	auto imgUpdate = ToPtr(new LambdaOp([&]() {
		static int f = 0;
		for (size_t i = 0; i < img.GetWidth(); i++) {
			for (size_t j = 0; j < img.GetHeight(); j++) {
				float r = 0.5 * i / (float)img.GetWidth();
				float g = 0.5 * j / (float)img.GetHeight();
				float b = 0.2 + 0.2*sinf(0.01f*f);
				img.SetPixel(img.GetWidth() - 1 - i, j, Image::Pixel<float>(r, g, b));
			}
		}
		f++;
	}, true));

	imgWindow.Run(imgUpdate);

	return 0;
	*/
}


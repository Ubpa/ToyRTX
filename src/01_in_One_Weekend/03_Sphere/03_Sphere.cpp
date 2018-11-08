#include <RayTracing/ImgWindow.h>
#include <RayTracing/RayCamera.h>

#include <Utility/Image.h>
#include <Utility/LambdaOp.h>

#include <RayTracing/Sphere.h>

#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace glm;
using namespace std;
typedef vec3 rgb;

rgb Scene(const Ray & ray);
rgb Background(const Ray & ray);

int main(int argc, char ** argv){
	ImgWindow::ENUM_OPTION option = static_cast<ImgWindow::ENUM_OPTION>(
		ImgWindow::ENUM_OPTION_SAVE_SRC_IMG
		| ImgWindow::ENUM_OPTION_POST_PROCESS_BLUR
	);
	ImgWindow imgWindow(str_WindowTitle, val_fps, option);
	if (!imgWindow.IsValid()) {
		printf("ERROR: Image Window Create Fail.\n");
		return 1;
	}

	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();
	const size_t val_ImgChannel = img.GetChannel();

	vec3 origin(0, 0, 0);
	vec3 viewPoint(0, 0, -1);
	float ratioWH = (float)val_ImgWidth / (float)val_ImgHeight;

	RayCamera camera(origin, viewPoint, ratioWH, 90.0f);

	auto imgUpdate = Operation::ToPtr(new LambdaOp([&]() {
		static size_t i = 0, j = 0;
		static double loopMax = 100;
		loopMax = 100 * imgWindow.GetScale();
		printf("loopMax: %.2f\n", loopMax);
		int cnt = 0;
		for (/*size_t i = 0*/; i < val_ImgWidth; i++) {
			for (/*size_t j = 0*/; j < val_ImgHeight;/*j++*/) {
				cnt++;
				float u = i / (float)val_ImgWidth;
				float v = j / (float)val_ImgHeight;
				Ray ray = camera.GenRay(u, v);
				rgb color = Scene(ray);
				float r = color.r;
				float g = color.g;
				float b = color.b;
				img.SetPixel(val_ImgWidth - 1 - i, j, Image::Pixel<float>(r, g, b));
				j++;
				if (cnt > loopMax)
					return;
			}
			j = 0;
		}
		imgWindow.SetImgUpdateOpDone();
	}, true));

	imgWindow.Run(imgUpdate);

	return 0;
}

rgb Scene(const Ray & ray) {
	static const Sphere sphere(vec3(0, 0, -1), 0.5f);
	Hitable::HitRecord record;
	if (sphere.Hit(ray, 0.001f, 9999.0f, record))
		return record.p;
	else
		return Background(ray);
}

rgb Background(const Ray & ray) {
	float t = 0.5*(normalize(ray.dir).y + 1.0f);
	rgb white = rgb(1.0f, 1.0f, 1.0f);
	rgb blue = rgb(0.5f, 0.7f, 1.0f);
	return (1 - t)*white + t * blue;
}

#include <RayTracing/ImgWindow.h>
#include <RayTracing/RayCamera.h>

#include <Utility/Image.h>
#include <Utility/LambdaOp.h>
#include <Utility/ImgPixelSet.h>

#include <RayTracing/Sphere.h>
#include <RayTracing/Group.h>

#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace glm;
using namespace std;
typedef vec3 rgb;

rgb Scene(Ptr<Hitable> scene, const Ray & ray);
rgb Background(const Ray & ray);

int main(int argc, char ** argv){
	ImgWindow::ENUM_OPTION option = static_cast<ImgWindow::ENUM_OPTION>(
		ImgWindow::ENUM_OPTION_SAVE_ALL_IMG
		//| ImgWindow::ENUM_OPTION_POST_PROCESS_FLIP
		| ImgWindow::ENUM_OPTION_POST_PROCESS_ALL
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

	ImgPixelSet pixelSet(val_ImgWidth, val_ImgHeight);
	//ImgPixelSet pixelSet;
	//pixelSet << vec2(val_ImgWidth / 2 * 1.05f, val_ImgHeight / 2);

	vec3 origin(0, 0, 0);
	vec3 viewPoint(0, 0, -1);
	float ratioWH = (float)val_ImgWidth / (float)val_ImgHeight;

	RayCamera camera(origin, viewPoint, ratioWH, 90.0f);

	auto sphere = Hitable::ToPtr(new Sphere(vec3(0, 0, -1), 0.5f));
	auto sphereBottom = Hitable::ToPtr(new Sphere(vec3(0, -100.5f, -1), 100.0f));
	auto group = Hitable::ToPtr(new Group);
	(*group) << sphere << sphereBottom;

	auto imgUpdate = Operation::ToPtr(new LambdaOp([&]() {
		static double loopMax = 100;
		static uniform_real_distribution<> randMap(0.0f,1.0f);
		static default_random_engine engine;
		loopMax = glm::max(100 * imgWindow.GetScale(), 1.0);
		int cnt = 0;
		while (pixelSet.Size() > 0) {
			auto pixel = pixelSet.RandPick();
			size_t i = pixel.x;
			size_t j = pixel.y;
			rgb color(0);
			const size_t sampleNum = 4;
			for (size_t k = 0; k < sampleNum; k++) {
				float u = (i + randMap(engine)) / (float)val_ImgWidth;
				float v = (j + randMap(engine)) / (float)val_ImgHeight;
				Ray ray = camera.GenRay(u, v);
				color += Scene(group, ray);
			}
			color /= sampleNum;
			float r = color.r;
			float g = color.g;
			float b = color.b;
			img.SetPixel(i, val_ImgHeight - 1 - j, Image::Pixel<float>(r, g, b));
			if (++cnt > loopMax)
				return;
		}
		imgWindow.SetImgUpdateOpDone();
	}, true));

	imgWindow.Run(imgUpdate);

	return 0;
}

rgb Scene(Ptr<Hitable> scene, const Ray & ray) {
	Hitable::HitRecord record;
	if (scene->Hit(ray, 10e-6f, FLT_MAX, record))
		return 0.5f*(record.normal + 1.0f);
	else
		return Background(ray);
}

rgb Background(const Ray & ray) {
	float t = 0.5*(normalize(ray.dir).y + 1.0f);
	rgb white = rgb(1.0f, 1.0f, 1.0f);
	rgb blue = rgb(0.5f, 0.7f, 1.0f);
	return (1 - t)*white + t * blue;
}

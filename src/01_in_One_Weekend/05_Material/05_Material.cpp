#include <RayTracing/RayTracer.h>
#include <RayTracing/Dielectric.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/OpMaterial.h>
#include <RayTracing/Sky.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/Group.h>
#include <RayTracing/ImgWindow.h>
#include <RayTracing/Ray.h>
#include <RayTracing/RayCamera.h>

#include <Utility/Math.h>
#include <Utility/ImgPixelSet.h>
#include <Utility/Image.h>
#include <Utility/Config.h>
#include <Utility/LambdaOp.h>
#include <Utility/GStorage.h>
#include <Utility/Timer.h>

#include <OpenGL/CommonDefine.h>

#include <omp.h>

#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace glm;
using namespace std;

Hitable::Ptr CreateScene();

int main(int argc, char ** argv){
	ImgWindow imgWindow(str_WindowTitle);
	if (!imgWindow.IsValid()) {
		printf("ERROR: Image Window Create Fail.\n");
		return 1;
	}

	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();
	const size_t val_ImgChannel = img.GetChannel();

	ImgPixelSet pixelSet(val_ImgWidth, val_ImgHeight);

	vec3 origin(-2, 2, 1);
	vec3 viewPoint(0, 0, -1);
	float ratioWH = (float)val_ImgWidth / (float)val_ImgHeight;

	RayCamera::Ptr camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, 45.0f, 0.2f));

	auto config = GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto pSampleNum = (*config)->GetIntPtr("sampleNum");
	const int sampleNum = pSampleNum ? *pSampleNum : 1;

	printf("INFO: cores : %d\n", omp_get_num_procs());
	omp_set_num_threads(omp_get_num_procs());
	vector<uvec2> pixels;

	auto scene = CreateScene();

	RayTracer rayTracer;
	Timer timer;
	timer.Start();
	size_t maxSumLoop = 5000;
	size_t curSumLoop = 0;
	Ptr<Operation> imgUpdate = ToPtr(new LambdaOp([&]() {
		//size_t curLoop = static_cast<size_t>(glm::max(imgWindow.GetScale(), 1.0));
		int imgSize = val_ImgWidth * val_ImgHeight;
#pragma omp parallel for schedule(dynamic, 1024)
		for (int pixelIdx = 0; pixelIdx < imgSize; pixelIdx++) {
			const uvec2 pixel(pixelIdx % val_ImgWidth, pixelIdx / val_ImgWidth);
			float u = (pixel.x + Math::Rand_F()) / (float)val_ImgWidth;
			float v = (pixel.y + Math::Rand_F()) / (float)val_ImgHeight;
			vec3 rst = rayTracer.TraceX(scene, camera->GenRay(u, v));

			auto _color = img.GetPixel_F(pixel.x, pixel.y);
			vec3 color(_color.r, _color.g, _color.b);
			vec3 newColor = (color*(float)curSumLoop + rst) / ((float)curSumLoop + 1);
			img.SetPixel(pixel.x, pixel.y, newColor);
		}
		curSumLoop++;
		double curStep = curSumLoop / (double)maxSumLoop * 100;
		double wholeTime = timer.GetWholeTime();
		double speed = curSumLoop / wholeTime;
		double needTime = (maxSumLoop - curSumLoop) / speed;
		double sumTime = wholeTime + needTime;
		printf("\rINFO: %.2f%%, %.2f loop / s, use %.2f s, need %.2f s, sum %.2f s     ",
			curStep, speed, wholeTime, needTime, sumTime);

		if (curSumLoop == maxSumLoop) {
			printf("\n");
			imgUpdate->SetIsHold(false);
		}
	}));

	bool success = imgWindow.Run(imgUpdate);
	return success ? 0 : 1;
}

Hitable::Ptr CreateScene(){
	auto skyMat = ToPtr(new OpMaterial([](const HitRecord & rec)->bool {
		float t = 0.5*(rec.vertex.pos.y + 1.0f);
		rgb white = rgb(1.0f, 1.0f, 1.0f);
		rgb blue = rgb(0.5f, 0.7f, 1.0f);
		rgb lightColor = (1 - t)*white + t * blue;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	auto sphereBottom = ToPtr(new Sphere(vec3(0, -100.5f, -1), 100.0f, ToPtr(new Lambertian(0.8f, 0.8f, 0.0f))));
	auto sphereMid = ToPtr(new Sphere(vec3(0, 0, -1), 0.5f, ToPtr(new Lambertian(0.1f, 0.2f, 0.5f))));
	auto sphereLeftOuter = ToPtr(new Sphere(vec3(-1, 0, -1), 0.5f, ToPtr(new Dielectric(1.5f, vec3(0)))));
	auto sphereLeftInner = ToPtr(new Sphere(vec3(-1, 0, -1), -0.45f, ToPtr(new Dielectric(1.5f, vec3(0)))));
	auto sphereRight = ToPtr(new Sphere(vec3(1, 0, -1), 0.5f, ToPtr(new Metal(0.8f, 0.6f, 0.2f, 0.2))));

	auto group = ToPtr(new Group);
	(*group) << sphereBottom << sphereMid << sphereLeftOuter << sphereLeftInner << sphereRight << sky;

	return group;
}
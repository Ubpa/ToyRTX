#include "CreateScene.h"

#include <RayTracing/RayTracer.h>
#include <RayTracing/ImgWindow.h>
#include <RayTracing/RayCamera.h>
#include <RayTracing/Ray.h>

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

int main(int argc, char ** argv){
	ImgWindow imgWindow(str_WindowTitle);
	if (!imgWindow.IsValid()) {
		printf("ERROR: Image Window Create Fail.\n");
		return 1;
	}

	printf("INFO: cores : %d\n", omp_get_num_procs());
	omp_set_num_threads(omp_get_num_procs());
	//omp_set_num_threads(1);

	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();

	ImgPixelSet pixelSet(val_ImgWidth, val_ImgHeight);
	//ImgPixelSet pixelSet;
	//pixelSet << vec2(val_ImgWidth / 2 * 0.65, val_ImgHeight / 2 * 0.65);

	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	int sampleNum;
	config->GetVal("sampleNum", sampleNum, 1);

	vector<uvec2> pixels;

	auto scene = CreateScene5((float)val_ImgWidth / (float)val_ImgHeight);

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
			vec3 rst = rayTracer.TraceX(scene->obj, scene->camera->GenRay(u, v));

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

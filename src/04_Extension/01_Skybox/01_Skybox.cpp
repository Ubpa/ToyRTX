#include "CreateScene.h"

#include <RayTracing/RayTracer.h>
#include <RayTracing/ImgWindow.h>

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
	//omp_set_num_threads(omp_get_num_procs());
	omp_set_num_threads(1);

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
	Ptr<Operation> imgUpdate = ToPtr(new LambdaOp([&]() {
		//size_t loopMax = static_cast<size_t>(glm::max(imgWindow.GetScale(), 1.0));
		size_t loopMax = 1000;
		pixelSet.RandPick(loopMax, pixels);

		int pixelsNum = pixels.size();
#pragma omp parallel for
		for (int pixelIdx = 0; pixelIdx < pixelsNum; pixelIdx++) {
			const uvec2 & pixel = pixels[pixelIdx];
			rgb color(0);
			for (int k = 0; k < sampleNum; k++) {
				float u = (pixel.x + Math::Rand_F()) / (float)val_ImgWidth;
				float v = (pixel.y + Math::Rand_F()) / (float)val_ImgHeight;
				color += rayTracer.TraceX(scene->obj, scene->camera->GenRay(u, v));
			}
			color /= sampleNum;
			img.SetPixel(pixel.x, val_ImgHeight - 1 - pixel.y, sqrt(color));
		}

		double curStep = 100 * (1 - pixelSet.Size() / double(val_ImgWidth * val_ImgHeight));
		double wholeTime = timer.GetWholeTime();
		double speed = (val_ImgWidth * val_ImgHeight - pixelSet.Size()) / wholeTime;
		double needTime = pixelSet.Size() / speed;
		double sumTime = wholeTime + needTime;
		printf("\rINFO: %.2f%%, %.2f sample / s, average depth %.2f, use %.2f s, need %.2f s, sum %.2f s     ",
			curStep, speed*sampleNum, rayTracer.depth/(speed*sampleNum*wholeTime), wholeTime, needTime, sumTime);

		if (pixelSet.Size() == 0) {
			printf("\n");
			imgUpdate->SetIsHold(false);
		}
	}));

	bool success = imgWindow.Run(imgUpdate);
	return success ? 0 : 1;
}

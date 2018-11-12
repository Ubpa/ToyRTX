#include <RayTracing/Dielectric.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/OpMaterial.h>
#include <RayTracing/Sky.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/Group.h>
#include <RayTracing/ImgWindow.h>
#include <RayTracing/RayCamera.h>

#include <Utility/Math.h>
#include <Utility/ImgPixelSet.h>
#include <Utility/Image.h>
#include <Utility/Config.h>
#include <Utility/LambdaOp.h>
#include <Utility/GStorage.h>

#include <OpenGL/CommonDefine.h>

#include <omp.h>

#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace glm;
using namespace std;
typedef vec3 rgb;

Hitable::Ptr CreateScene();
rgb RayTracer(const Hitable::Ptr & scene, Ray::Ptr & ray, size_t depth = 20);
rgb Background(const Ray::Ptr & ray);

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

	RayCamera::Ptr camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, 0.2f, 45.0f));

	auto config = GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto pSampleNum = (*config)->GetIntPtr("sampleNum");
	const int sampleNum = pSampleNum ? *pSampleNum : 1;

	printf("INFO: cores : %d\n", omp_get_num_procs());
	omp_set_num_threads(omp_get_num_procs());
	vector<uvec2> pixels;

	auto scene = CreateScene();

	Ptr<Operation> imgUpdate = ToPtr(new LambdaOp([&]() {
		static double loopMax = 100;
		static uniform_real_distribution<> randMap(0.0f,1.0f);
		static default_random_engine engine;
		loopMax = glm::max(100 * imgWindow.GetScale(), 1.0);
		int cnt = 0;

		pixelSet.RandPick(loopMax, pixels);
		int pixelsNum = pixels.size();
		#pragma omp parallel for
		for (int pixelIdx = 0; pixelIdx < pixelsNum; pixelIdx++) {
			const uvec2 & pixel = pixels[pixelIdx];
			rgb color(0);
			for (int k = 0; k < sampleNum; k++) {
				float u = (pixel.x + randMap(engine)) / (float)val_ImgWidth;
				float v = (pixel.y + randMap(engine)) / (float)val_ImgHeight;
				Ray::Ptr ray = camera->GenRay(u, v);
				color += RayTracer(scene, ray);
			}
			color /= sampleNum;
			img.SetPixel(pixel.x, val_ImgHeight - 1 - pixel.y, sqrt(color));
		}

		printf("\r%.2f%%", 100 * (1 - pixelSet.Size() / float(val_ImgWidth * val_ImgHeight)));
		if (pixelSet.Size() == 0) {
			printf("\n");
			imgUpdate->SetIsHold(false);
		}
	}, true));

	auto success = imgWindow.Run(imgUpdate);
	return success ? 0 : 1;
}

Hitable::Ptr CreateScene(){
	auto skyMat = ToPtr(new OpMaterial([](HitRecord & rec)->bool {
		float t = 0.5*(rec.pos.y + 1.0f);
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

rgb RayTracer(const Hitable::Ptr & scene, Ray::Ptr & ray, size_t depth) {
	if(depth == 0)
		return rgb(10e-6);

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.hitable->RayOut(hitRst.record))
			return RayTracer(scene, ray, depth-1);
		else
			return ray->GetColor();
	}
	else
		return rgb(10e-6);
}

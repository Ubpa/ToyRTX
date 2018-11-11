#include <RayTracing/Scene.h>
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
#include <Utility/Timer.h>

#include <OpenGL/CommonDefine.h>

#include <omp.h>

#include "Defines.h"

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace Define;
using namespace glm;
using namespace std;
typedef vec3 rgb;

Scene::Ptr CreateScene(float ratioWH);
rgb RayTracer(const Hitable::Ptr & scene, Ray::Ptr & ray, size_t depth = 50);
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
	const float val_RatioWH = (float)val_ImgWidth / (float)val_ImgHeight;

	ImgPixelSet pixelSet(val_ImgWidth, val_ImgHeight);
	//ImgPixelSet pixelSet;
	//pixelSet << vec2(val_ImgWidth / 2, val_ImgHeight / 4*1.05f);

	auto config = GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto pSampleNum = (*config)->GetIntPtr("sampleNum");
	const int sampleNum = pSampleNum ? *pSampleNum : 1;

	printf("INFO: cores : %d\n", omp_get_num_procs());
	omp_set_num_threads(omp_get_num_procs());
	vector<vec2> pixels;

	auto scene = CreateScene(val_RatioWH);

	Timer timer;
	timer.Start();
	Ptr<Operation> imgUpdate = ToPtr(new LambdaOp([&]() {
		static double loopMax = 100;
		static uniform_real_distribution<> randMap(0.0f,1.0f);
		static default_random_engine engine;
		loopMax = glm::max(100 * imgWindow.GetScale(), 1.0);
		
		pixelSet.RandPick(loopMax, pixels);
		int pixelsNum = pixels.size();

#pragma omp parallel for
		for (int pixelIdx = 0; pixelIdx < pixelsNum; pixelIdx++) {
			vec2 pixel = pixels[pixelIdx];
			rgb color(0);
			for (int k = 0; k < sampleNum; k++) {
				float u = (pixel.x + randMap(engine)) / (float)val_ImgWidth;
				float v = (pixel.y + randMap(engine)) / (float)val_ImgHeight;
				Ray::Ptr ray = scene->camera->GenRay(u, v);
				color += RayTracer(scene->obj, ray);
			}
			color /= sampleNum;
			img.SetPixel(pixel.x, val_ImgHeight - 1 - pixel.y, sqrt(color));
		}

		float curStep = 100 * (1 - pixelSet.Size() / float(val_ImgWidth * val_ImgHeight));
		float t = timer.Log();
		static float speed = 0;
		static size_t cnt = 0;
		speed = (speed * cnt + pixelsNum / t) / (cnt + 1);
		cnt++;
		float wholeTime = timer.GetWholeTime();
		float needTime = pixelSet.Size() / speed;
		printf("\r%.2f%%, %.2f pixle / s, use %.2f s, need %.2f s, sum %.2f s", curStep, speed, wholeTime, needTime, wholeTime + needTime);
		if (pixelSet.Size() == 0) {
			printf("\n");
			imgUpdate->SetIsHold(false);
		}
	}, true));

	auto success = imgWindow.Run(imgUpdate);
	return success ? 0 : 1;
}

Scene::Ptr CreateScene(float ratioWH){
	auto skyMat = ToPtr(new OpMaterial([](HitRecord & rec)->bool {
		float t = 0.5 * (rec.pos.y + 1.0f);
		rgb white = rgb(1.0f, 1.0f, 1.0f);
		rgb blue = rgb(0.5f, 0.7f, 1.0f);
		rgb lightColor = (1 - t) * white + t * blue;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	auto group = ToPtr(new Group);
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Math::Rand_F();
			vec3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					auto mat = ToPtr(new Lambertian(vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F())));
					auto sphere = ToPtr(new Sphere(center, 0.2, mat));
					group->push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToPtr(new Metal(vec3(0.5*(1 + Math::Rand_F()), 0.5*(1 + Math::Rand_F()), 0.5*(1 + Math::Rand_F())), 0.5*Math::Rand_F()));
					auto sphere = ToPtr(new Sphere(center, 0.2, mat));
					group->push_back(sphere);
				}
				else {  // glass
					auto mat = ToPtr(new Dielectric(1.5));
					auto sphere = ToPtr(new Sphere(center, 0.2, mat));
					group->push_back(sphere);
				}
			}
		}
	}
	
	auto sphereBottom = ToPtr(new Sphere(vec3(0, -1000, 0), 1000, ToPtr(new Lambertian(vec3(0.5, 0.5, 0.5)))));
	auto sphere0 = ToPtr(new Sphere(vec3(6, 1, 0), 1.0, ToPtr(new Metal(vec3(0.7, 0.6, 0.5), 0.0))));
	auto sphere1 = ToPtr(new Sphere(vec3(2, 1, 0), 1.0, ToPtr(new Dielectric(1.5))));
	auto sphere2 = ToPtr(new Sphere(vec3(2, 1, 0), -0.8, ToPtr(new Dielectric(1.5))));
	auto sphere3 = ToPtr(new Sphere(vec3(-2, 1, 0), 1.0, ToPtr(new Lambertian(vec3(0.4, 0.2, 0.1)))));
	auto sphere4 = ToPtr(new Sphere(vec3(-6, 1, 0), 1.0, ToPtr(new Dielectric(2.5))));

	(*group) << sphere0 << sphere1 << sphere2 << sphere3 << sphere4 << sphereBottom << sky;

	vec3 origin(13, 2, 3);
	vec3 viewPoint(0, 0, 0);
	float lenR = 0.05;
	float distToFocus = 10.0f;
	RayCamera::Ptr camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, lenR, 20.0, distToFocus));

	return ToPtr(new Scene(group, camera));
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

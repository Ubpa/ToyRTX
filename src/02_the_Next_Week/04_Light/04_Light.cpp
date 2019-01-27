#include <CppUtil/RTX/Light.h>
#include <CppUtil/RTX/ImgTexture.h>
#include <CppUtil/RTX/RayTracer.h>
#include <CppUtil/RTX/OpTexture.h>
#include <CppUtil/RTX/BVH_Node.h>
#include <CppUtil/RTX/MoveSphere.h>
#include <CppUtil/RTX/Scene.h>
#include <CppUtil/RTX/Dielectric.h>
#include <CppUtil/RTX/Metal.h>
#include <CppUtil/RTX/Lambertian.h>
#include <CppUtil/RTX/OpMaterial.h>
#include <CppUtil/RTX/Sky.h>
#include <CppUtil/RTX/Sphere.h>
#include <CppUtil/RTX/Group.h>
#include <CppUtil/RTX/ImgWindow.h>
#include <CppUtil/RTX/TRayCamera.h>
#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/ImgPixelSet.h>
#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Config.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Timer.h>

#include <CppUtil/OpenGL/CommonDefine.h>

#include <omp.h>

#include "Defines.h"

using namespace CppUtil::Basic;
using namespace RTX;
using namespace Define;
using namespace glm;
using namespace std;

Scene::Ptr CreateScene0(float ratioWH);
Scene::Ptr CreateScene1(float ratioWH);
rgb Background(CppUtil::Basic::Ptr<Ray> ray);

int main(int argc, char ** argv){
	ImgWindow imgWindow(str_WindowTitle);
	if (!imgWindow.IsValid()) {
		printf("ERROR: Image Window Create Fail.\n");
		return 1;
	}

	printf("INFO: cores : %d\n", omp_get_num_procs());
	omp_set_num_threads(omp_get_num_procs());

	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();

	ImgPixelSet pixelSet(val_ImgWidth, val_ImgHeight);

	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	int sampleNum;
	config->GetVal("sampleNum", sampleNum, 1);

	vector<uvec2> pixels;

	auto scene = CreateScene1((float)val_ImgWidth / (float)val_ImgHeight);

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

Scene::Ptr CreateScene0(float ratioWH){
	auto skyMat = ToPtr(new OpMaterial([](const HitRecord & rec)->bool {
		float t = 0.5f * (rec.vertex.pos.y + 1.0f);
		rgb white = rgb(1.0f, 1.0f, 1.0f);
		rgb blue = rgb(0.5f, 0.7f, 1.0f);
		rgb lightColor = (1 - t) * white + t * blue;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));
	
	float t0 = 0.0f;
	float t1 = 1.0f;
	
	vector<Hitable::CPtr> bvhData;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Math::Rand_F();
			vec3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					auto mat = ToPtr(new Lambertian(vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F())));
					auto sphere = ToPtr(new MoveSphere(t0, t1, center, center+vec3(0,Math::Rand_F()*0.5,0), 0.2, mat));
					bvhData.push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToPtr(new Metal(vec3(0.5f*(1.0f + Math::Rand_F()), 0.5f*(1.0f + Math::Rand_F()), 0.5f*(1.0f + Math::Rand_F())), 0.5f*Math::Rand_F()));
					auto sphere = ToPtr(new Sphere(center, 0.2, mat));
					bvhData.push_back(sphere);
				}
				else {  // glass
					auto mat = ToPtr(new Dielectric(1.5));
					auto sphere = ToPtr(new Sphere(center, 0.2, mat));
					bvhData.push_back(sphere);
				}
			}
		}
	}

	auto bvhNode = ToPtr(new BVH_Node(bvhData));

	auto checkTex = OpTexture::CheckerTexture(rgb(0.2, 0.3, 0.1), rgb(0.9, 0.9, 0.9));
	auto noiseTex = OpTexture::NoiseTexture(0, vec3(1), 3);
	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto rootPath = *config->GetStrPtr("RootPath");
	auto earthTex = ToPtr(new ImgTexture(rootPath + str_Img_Earth, true));
	if (!earthTex->IsValid()) {
		printf("ERROR: earthTex[%s] is invalid.\n", (str_RootPath + str_Img_Earth).c_str());
		exit(1);
	}

	auto group = ToPtr(new Group);
	auto sphereBottom = ToPtr(new Sphere(vec3(0, -1000, 0), 1000, ToPtr(new Lambertian(noiseTex))));
	auto sphere0 = ToPtr(new Sphere(vec3(6, 1, 0), 1.0, ToPtr(new Metal(vec3(0.7, 0.6, 0.5), 0.0))));
	auto sphere1 = ToPtr(new Sphere(vec3(2, 1, 0), 1.0, ToPtr(new Dielectric(1.5))));
	auto sphere2 = ToPtr(new Sphere(vec3(2, 1, 0), -0.8, ToPtr(new Dielectric(1.5))));
	auto sphere3 = ToPtr(new Sphere(vec3(-2, 1, 0), 1.0, ToPtr(new Lambertian(earthTex))));
	auto sphere4 = ToPtr(new Sphere(vec3(-6, 1, 0), 1.0, ToPtr(new Lambertian(checkTex))));

	(*group) << bvhNode << sphere0 << sphere1 << sphere2 << sphere3 << sphere4 << sphereBottom << sky;

	vec3 origin(13, 2, 3);
	vec3 viewPoint(0, 0, 0);
	float fov = 20.0f;
	float lenR = 0.05f;
	float distToFocus = 10.0f;
	TRayCamera::Ptr camera = ToPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));
	
	return ToPtr(new Scene(group, camera));
}

Scene::Ptr CreateScene1(float ratioWH) {
	auto skyMat = ToPtr(new OpMaterial([](const HitRecord & rec)->bool {
		float t = 0.5f * (rec.vertex.pos.y + 1.0f);
		rgb c0 = rgb(0.005f);
		rgb c1 = c0 * rgb(0.75f, 0.5f, 0.375f);
		rgb lightColor = (1 - t) * c0 + t * c1;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	auto group = ToPtr(new Group);

	auto noiseMat = ToPtr(new Lambertian(OpTexture::NoiseTexture(0, vec3(1), 3)));
	auto redLightMat = ToPtr(new Light(rgb(1.0f, 0, 0)));
	auto greenLightMat = ToPtr(new Light(rgb(0, 1.0f, 0)));
	auto blueLightMat = ToPtr(new Light(rgb(0, 0, 1.0f)));
	auto whiteLightMat = ToPtr(new Light(rgb(1.0f)));

	auto sphere00 = ToPtr(new Sphere(vec3(0, 1, 0), 1.0, ToPtr(new Dielectric(1.5))));
	auto sphere01 = ToPtr(new Sphere(vec3(0, 1, 0), -0.8, ToPtr(new Dielectric(1.5))));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -1000, 0), 1000.0f, noiseMat));
	auto sphere2 = ToPtr(new Sphere(vec3(-2, 2, 1.732), 0.5f, redLightMat));
	auto sphere3 = ToPtr(new Sphere(vec3(2, 2, 1.732), 0.5f, greenLightMat));
	auto sphere4 = ToPtr(new Sphere(vec3(0, 2, -1.732), 0.5f, blueLightMat));
	auto sphere5 = ToPtr(new Sphere(vec3(0, 3, 0), 0.5f, whiteLightMat));

	(*group) << sphere00 << sphere01 << sphere1 << sphere2 << sphere3 << sphere4 << sphere5 << sky;

	float t0 = 0.0f;
	float t1 = 1.0f;
	vec3 origin(0, 1, 5);
	vec3 viewPoint(0, 1, 0);
	float fov = 45.0f;
	float lenR = 0.05f;
	float distToFocus = 5.0f;
	TRayCamera::Ptr camera = ToPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));

	return ToPtr(new Scene(group, camera));
}
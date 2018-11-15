#include "CreateScene.h"

#include <RayTracing/Transform.h>
#include <RayTracing/TriMesh.h>
#include <RayTracing/Light.h>
#include <RayTracing/Group.h>
#include <RayTracing/OpTexture.h>
#include <RayTracing/BVH_Node.h>
#include <RayTracing/MoveSphere.h>
#include <RayTracing/Dielectric.h>
#include <RayTracing/Sky.h>
#include <RayTracing/OpMaterial.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/ImgTexture.h>
#include <RayTracing/TRayCamera.h>

#include <Utility/Config.h>
#include <Utility/Math.h>
#include <Utility/GStorage.h>

#include "Defines.h"
#include <OpenGL/CommonDefine.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace Define;
using namespace glm;
using namespace std;

Scene::Ptr CreateScene0(float ratioWH) {
	auto skyMat = ToPtr(new OpMaterial([](HitRecord & rec)->bool {
		float t = 0.5 * (rec.vertex.pos.y + 1.0f);
		rgb white = rgb(1.0f, 1.0f, 1.0f);
		rgb blue = rgb(0.5f, 0.7f, 1.0f);
		rgb lightColor = (1 - t) * white + t * blue;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	float t0 = 0.0f;
	float t1 = 1.0f;

	vector<Hitable::Ptr> bvhData;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Math::Rand_F();
			vec3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					auto mat = ToPtr(new Lambertian(vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F())));
					auto sphere = ToPtr(new MoveSphere(t0, t1, center, center + vec3(0, Math::Rand_F()*0.5, 0), 0.2, mat));
					bvhData.push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToPtr(new Metal(vec3(0.5*(1 + Math::Rand_F()), 0.5*(1 + Math::Rand_F()), 0.5*(1 + Math::Rand_F())), 0.5*Math::Rand_F()));
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
	auto skyMat = ToPtr(new OpMaterial([](HitRecord & rec)->bool {
		float t = 0.5 * (rec.vertex.pos.y + 1.0f);
		rgb c0 = rgb(0.005f);
		rgb c1 = c0 * rgb(0.75f, 0.5f, 0.375f);
		rgb lightColor = (1 - t) * c0 + t * c1;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	auto group = ToPtr(new Group);

	auto noiseMat = ToPtr(new Lambertian(OpTexture::NoiseTexture(0, vec3(1), 3)));

	auto sphere00 = ToPtr(new Sphere(vec3(0, 1, 0), 1.0, ToPtr(new Dielectric(1.5))));
	auto sphere01 = ToPtr(new Sphere(vec3(0, 1, 0), -0.8, ToPtr(new Dielectric(1.5))));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -1000, 0), 1000.0f, noiseMat));

	vector<Vertex> vertexs;
	for (size_t i = 0; i < sizeof(data_RectVertexPos) / sizeof(float); i += 3)
		vertexs.push_back(Vertex(vec3(data_RectVertexPos[i], data_RectVertexPos[i + 1], data_RectVertexPos[i + 2])));

	auto lightMat = ToPtr(new Light(vec3(1)));
	auto triMesh = ToPtr(new TriMesh(vertexs, lightMat));
	if (!triMesh->IsValid()) {
		printf("ERROR: triMesh is invalid.\n");
		exit(1);
	}
	mat4 tfm(1.0f);
	tfm = translate(tfm, vec3(2, 1.5, 0));
	tfm = scale(tfm, vec3(1, 2, 1));
	tfm = rotate(tfm, Math::PI / 2, vec3(0, 1, 0));
	auto tfmL = ToPtr(new Transform(tfm, triMesh));

	(*group) << sphere00 << sphere01 << sphere1 << tfmL << sky;

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

Scene::Ptr CreateScene2(float ratioWH) {
	auto skyMat = ToPtr(new OpMaterial([](HitRecord & rec)->bool {
		float t = 0.5 * (rec.vertex.pos.y + 1.0f);
		rgb c0 = rgb(1.0f);
		rgb c1 = c0 * rgb(0.5,0.7,1.0);
		rgb lightColor = (1 - t) * c0 + t * c1;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	vector<Vertex> vertexs;
	for (size_t i = 0; i < sizeof(data_RectVertexPos) / sizeof(float); i += 6) {
		vec3 pos(data_RectVertexPos[i], data_RectVertexPos[i + 1], data_RectVertexPos[i + 2]);
		vec3 normal(data_RectVertexPos[i+3], data_RectVertexPos[i + 4], data_RectVertexPos[i + 5]);
		vertexs.push_back(Vertex());
	}

	auto redMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(1.0, 0, 0))));
	auto redWall = ToPtr(new TriMesh(vertexs, redMat));
	if (!redWall->IsValid()) {
		printf("ERROR: redWall is invalid.\n");
		exit(1);
	}
	mat4 tfmRed(1.0f);
	tfmRed = translate(tfmRed, vec3(3, 0, 0));
	tfmRed = scale(tfmRed, vec3(6));
	tfmRed = rotate(tfmRed, Math::PI / 2, vec3(0, 1, 0));
	auto tfmRedWall = ToPtr(new Transform(tfmRed, redWall));

	auto greenMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0, 1.0f, 0))));
	auto greenWall = ToPtr(new TriMesh(vertexs, greenMat));
	if (!greenWall->IsValid()) {
		printf("ERROR: greenWall is invalid.\n");
		exit(1);
	}
	mat4 tfmGreen(1.0f);
	tfmGreen = translate(tfmGreen, vec3(-3, 0, 0));
	tfmGreen = scale(tfmGreen, vec3(6));
	tfmGreen = rotate(tfmGreen, Math::PI / 2, vec3(0, 1, 0));
	auto tfmGreenWall = ToPtr(new Transform(tfmGreen, greenWall));

	auto grayMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.8))));
	auto grayWall = ToPtr(new TriMesh(vertexs, grayMat));
	if (!grayWall->IsValid()) {
		printf("ERROR: grayWall is invalid.\n");
		exit(1);
	}
	mat4 tfm1Gray(1.0f);
	tfm1Gray = translate(tfm1Gray, vec3(0, -3, 0));
	tfm1Gray = scale(tfm1Gray, vec3(6));
	tfm1Gray = rotate(tfm1Gray, Math::PI / 2, vec3(1, 0, 0));
	auto tfm1GrayWall = ToPtr(new Transform(tfm1Gray, grayWall));

	mat4 tfm2Gray(1.0f);
	tfm2Gray = translate(tfm2Gray, vec3(0, 3, 0));
	tfm2Gray = scale(tfm2Gray, vec3(6));
	tfm2Gray = rotate(tfm2Gray, Math::PI / 2, vec3(1, 0, 0));
	auto tfm2GrayWall = ToPtr(new Transform(tfm2Gray, grayWall));

	mat4 tfm3Gray(1.0f);
	tfm3Gray = translate(tfm3Gray, vec3(0, 0, -3));
	tfm3Gray = scale(tfm3Gray, vec3(6));
	auto tfm3GrayWall = ToPtr(new Transform(tfm3Gray, grayWall));

	auto whiteLightMat = ToPtr(new Light(rgb(0.5f)));
	auto sphere = ToPtr(new Sphere(vec3(0, 0, 0), 0.5f, whiteLightMat));

	auto group = ToPtr(new Group);

	(*group) /*<< tfmRedWall << tfmGreenWall << tfm1GrayWall << tfm2GrayWall << tfm3GrayWall*/ << sphere;// << sky;

	vector<Vertex> vertexs2;
	for (size_t i = 0; i < sizeof(data_Rect2VertexPos) / sizeof(float); i += 3)
		vertexs2.push_back(Vertex(vec3(data_Rect2VertexPos[i], data_Rect2VertexPos[i + 1], data_Rect2VertexPos[i + 2])));
	auto grayMat2 = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.8))));
	auto grayWall2 = ToPtr(new TriMesh(vertexs2, grayMat2));
	if (!grayWall2->IsValid()) {
		printf("ERROR: grayWall is invalid.\n");
		exit(1);
	}

	(*group) << grayWall2;

	float t0 = 0.0f;
	float t1 = 1.0f;
	vec3 origin(0, 0, 15);
	vec3 viewPoint(0, 0, 0);
	float fov = 45.0f;
	float lenR = 0.05f;
	float distToFocus = 8.0f;
	TRayCamera::Ptr camera = ToPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));

	return ToPtr(new Scene(group, camera));
}
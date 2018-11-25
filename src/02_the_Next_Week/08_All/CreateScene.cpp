#include "CreateScene.h"

#include <RayTracing/Isotropic.h>
#include <RayTracing/Volume.h>
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

Scene::CPtr CreateScene0(float ratioWH) {
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
					auto sphere = ToPtr(new MoveSphere(t0, t1, center, center + vec3(0, Math::Rand_F()*0.5f, 0), 0.2f, mat));
					bvhData.push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToPtr(new Metal(vec3(0.5f*(1.0f + Math::Rand_F()), 0.5f*(1.0f + Math::Rand_F()), 0.5f*(1.0f + Math::Rand_F())), 0.5f*Math::Rand_F()));
					auto sphere = ToPtr(new Sphere(center, 0.2f, mat));
					bvhData.push_back(sphere);
				}
				else {  // glass
					auto mat = ToPtr(new Dielectric(1.5));
					auto sphere = ToPtr(new Sphere(center, 0.2f, mat));
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
	auto sphere1 = ToPtr(new Sphere(vec3(2, 1, 0), 1.0, ToPtr(new Dielectric(1.5f))));
	auto sphere2 = ToPtr(new Sphere(vec3(2, 1, 0), -0.8f, ToPtr(new Dielectric(1.5f))));
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

Scene::CPtr CreateScene1(float ratioWH) {
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

	auto sphere00 = ToPtr(new Sphere(vec3(0, 1, 0), 1.0f, ToPtr(new Dielectric(1.5f))));
	auto sphere01 = ToPtr(new Sphere(vec3(0, 1, 0), -0.8f, ToPtr(new Dielectric(1.5f))));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -1000, 0), 1000.0f, noiseMat));

	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 3)
		squareVertexs.push_back(Vertex(vec3(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2])));

	auto lightMat = ToPtr(new Light(vec3(1)));
	auto triMesh = ToPtr(new TriMesh(squareVertexs, lightMat));
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

Scene::CPtr CreateScene2(float ratioWH) {
	// Mesh
	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 6) {
		vec3 pos(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2]);
		vec3 normal(data_SquareVertexPos[i+3], data_SquareVertexPos[i + 4], data_SquareVertexPos[i + 5]);
		squareVertexs.push_back(Vertex(pos, normal));
	}
	auto square = ToPtr(new TriMesh(squareVertexs));
	if (!square->IsValid()) {
		printf("ERROR: square is invalid.\n");
		exit(1);
	}

	vector<Vertex> cubeVertexs;
	for (size_t i = 0; i < sizeof(data_CubeVertexPos) / sizeof(float); i += 6) {
		vec3 pos(data_CubeVertexPos[i], data_CubeVertexPos[i + 1], data_CubeVertexPos[i + 2]);
		vec3 normal(data_CubeVertexPos[i + 3], data_CubeVertexPos[i + 4], data_CubeVertexPos[i + 5]);
		cubeVertexs.push_back(Vertex(pos, normal));
	}
	auto cube = ToPtr(new TriMesh(cubeVertexs));
	if (!cube->IsValid()) {
		printf("ERROR: cube is invalid.\n");
		exit(1);
	}

	// Mat
	auto redMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.65f, 0.05f, 0.05f))));
	auto greenMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.12f, 0.45f, 0.15f))));
	auto grayMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.73f, 0.73f, 0.73f))));
	auto lightMat = ToPtr(new Light(rgb(3.0f)));
	auto cubeMat = ToPtr(new Lambertian(rgb(1.0f)));
	auto volumnMat = ToPtr(new Isotropic(vec3(1.0f)));

	// Transform
	mat4 tfmRight(1.0f);
	tfmRight = translate(tfmRight, vec3(3, 0, 0));
	tfmRight = scale(tfmRight, vec3(6));
	tfmRight = rotate(tfmRight, - Math::PI / 2, vec3(0, 1, 0));
	auto redWall = ToPtr(new Transform(tfmRight, square, redMat));

	mat4 tfmLeft(1.0f);
	tfmLeft = translate(tfmLeft, vec3(-3, 0, 0));
	tfmLeft = scale(tfmLeft, vec3(6));
	tfmLeft = rotate(tfmLeft, Math::PI / 2, vec3(0, 1, 0));
	auto greenWall = ToPtr(new Transform(tfmLeft, square, greenMat));
	
	mat4 tfmBottom(1.0f);
	tfmBottom = translate(tfmBottom, vec3(0, -3, 0));
	tfmBottom = scale(tfmBottom, vec3(6));
	tfmBottom = rotate(tfmBottom, - Math::PI / 2, vec3(1, 0, 0));
	auto bottomWall = ToPtr(new Transform(tfmBottom, square, grayMat));

	mat4 tfmTop(1.0f);
	tfmTop = translate(tfmTop, vec3(0, 3, 0));
	tfmTop = scale(tfmTop, vec3(6));
	tfmTop = rotate(tfmTop, Math::PI / 2, vec3(1, 0, 0));
	auto topWall = ToPtr(new Transform(tfmTop, square, grayMat));

	mat4 tfmBack(1.0f);
	tfmBack = translate(tfmBack, vec3(0, 0, -3));
	tfmBack = scale(tfmBack, vec3(6));
	auto backWall = ToPtr(new Transform(tfmBack, square, grayMat));
	
	mat4 tfmLight(1.0f);
	tfmLight = translate(tfmLight, vec3(0, 2.999, 0));
	tfmLight = scale(tfmLight, vec3(4));
	tfmLight = rotate(tfmLight, Math::PI / 2, vec3(1, 0, 0));
	auto light = ToPtr(new Transform(tfmLight, square, lightMat));

	mat4 tfmCube1(1.0f);
	tfmCube1 = translate(tfmCube1, vec3(1.1, -2.2, 1));
	tfmCube1 = scale(tfmCube1, vec3(1.6f));
	tfmCube1 = rotate(tfmCube1, - Math::PI / 12, vec3(0, 1, 0));
	auto cube1 = ToPtr(new Transform(tfmCube1, cube, cubeMat));

	mat4 tfmCube2(1.0f);
	tfmCube2 = translate(tfmCube2, vec3(-1.1, -1.5, -0.5));
	tfmCube2 = scale(tfmCube2, vec3(1.6,3,1.6));
	tfmCube2 = rotate(tfmCube2, Math::PI / 9, vec3(0, 1, 0));
	//tfmCube2 = scale(tfmCube2, vec3(4));
	auto volumn = ToPtr(new Volume(cube, 3.0f, volumnMat));
	auto cube2 = ToPtr(new Transform(tfmCube2, volumn));
	

	// Scene
	auto group = ToPtr(new Group);

	(*group) << greenWall << redWall << bottomWall << topWall << backWall << cube1 << cube2 << light;

	// Camera
	float t0 = 0.0f;
	float t1 = 1.0f;
	vec3 origin(0, 0, 10);
	vec3 viewPoint(0, 0, 0);
	float fov = 45.0f;
	float lenR = 0.05f;
	float distToFocus = 8.0f;
	TRayCamera::Ptr camera = ToPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));

	return ToPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene3(float ratioWH) {
	// Camera
	float t0 = 0.0f;
	float t1 = 1.0f;
	vec3 origin(478, 278, -600);
	vec3 viewPoint(278, 278, 0);
	float fov = 40.0f;
	float lenR = 0.0f;
	float distToFocus = 10.0f;
	TRayCamera::Ptr camera = ToPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));

	// Mesh
	// Mesh Square
	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 6) {
		vec3 pos(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2]);
		vec3 normal(data_SquareVertexPos[i + 3], data_SquareVertexPos[i + 4], data_SquareVertexPos[i + 5]);
		squareVertexs.push_back(Vertex(pos, normal));
	}
	auto square = ToPtr(new TriMesh(squareVertexs));
	if (!square->IsValid()) {
		printf("ERROR: square is invalid.\n");
		exit(1);
	}

	// Mesh Cube
	vector<Vertex> cubeVertexs;
	for (size_t i = 0; i < sizeof(data_CubeVertexPos) / sizeof(float); i += 6) {
		vec3 pos(data_CubeVertexPos[i], data_CubeVertexPos[i + 1], data_CubeVertexPos[i + 2]);
		vec3 normal(data_CubeVertexPos[i + 3], data_CubeVertexPos[i + 4], data_CubeVertexPos[i + 5]);
		cubeVertexs.push_back(Vertex(pos, normal));
	}
	auto cube = ToPtr(new TriMesh(cubeVertexs));
	if (!cube->IsValid()) {
		printf("ERROR: cube is invalid.\n");
		exit(1);
	}

	// Ground
	int nb = 20;
	auto groundMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.48f, 0.83f, 0.53f))));
	vector<Hitable::CPtr> boxs;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (Math::Rand_F() + 0.01f);
			float z1 = z0 + w;
			vec3 minP = vec3(x0, y0, z0);
			vec3 maxP = vec3(x1, y1, z1);
			vec3 center = (minP + maxP) / 2.0f;
			vec3 scaleXYZ = maxP - minP;
			mat4 tfm(1.0f);
			tfm = translate(tfm, center);
			tfm = scale(tfm, scaleXYZ);
			auto box = ToPtr(new Transform(tfm, cube));
			boxs.push_back(box);
		}
	}
	auto boxBVH = ToPtr(new BVH_Node(boxs, groundMat));

	// Light
	// x0 = 123, x1 = 423, z0 = 147, z1 = 412, y = 554
	auto lightMat = ToPtr(new Light(vec3(7)));
	mat4 lightTfm(1.0f);
	lightTfm = translate(lightTfm, vec3((123+423)/2, 554, (147+412)/2));
	lightTfm = rotate(lightTfm, Math::PI / 2, vec3(1, 0, 0));
	lightTfm = scale(lightTfm, vec3(423 - 123, 412 - 127, 1.00));
	auto light = ToPtr(new Transform(lightTfm, square, lightMat));

	// Move Sphere
	vec3 center(400, 400, 200);
	auto moveSphereMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.7, 0.3, 0.1))));
	auto moveSphere = ToPtr(new MoveSphere(t0, t1, center, center + vec3(30, 0, 0), 50, moveSphereMat));

	// Dielectric, Metal
	auto sphere1 = ToPtr(new Sphere(vec3(260, 150, 45), 50, ToPtr(new Dielectric(1.5f))));
	auto sphere2 = ToPtr(new Sphere(vec3(0, 150, 145), 50, ToPtr(new Metal(vec3(0.8f, 0.8f, 0.9f), 10.0f))));

	// Sphere Volume with Dielelctric Boundary
	auto sphereBoundary = ToPtr(new Sphere(vec3(360, 150, 145), 70, ToPtr(new Dielectric(1.5f/*, vec3(0.01, 1, 0.01)/*/))));
	auto sphereVolume = ToPtr(new Volume(sphereBoundary, 0.2f, ToPtr(new Isotropic(rgb(0.2f,0.4,0.9f)))));

	// Air
	auto airBoundary = ToPtr(new Sphere(vec3(0), 5000));
	auto airVolume = ToPtr(new Volume(airBoundary, 0.0001f, ToPtr(new Isotropic(rgb(1.0f)))));

	// Earth
	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto rootPath = *config->GetStrPtr("RootPath");
	auto earthTex = ToPtr(new ImgTexture(rootPath + str_Img_Earth, true));
	if (!earthTex->IsValid()) {
		printf("Error: earthTex is not Valid.\n");
		exit(1);
	}
	auto earthMat = ToPtr(new Lambertian(earthTex));
	auto earth = ToPtr(new Sphere(vec3(400, 200, 400), 100, earthMat));

	// Noise Sphere
	auto noiseMat = ToPtr(new Lambertian(OpTexture::NoiseTexture(0, vec3(1), 0.1)));
	auto noiseSphere = ToPtr(new Sphere(vec3(220, 280, 300), 80, noiseMat));

	// Balls Cube
	vector<Hitable::CPtr> balls;
	size_t ns = 1000;
	for (size_t j = 0; j < ns; j++) {
		vec3 center = 165.0f * vec3(Math::Rand_F(), Math::Rand_F(), Math::Rand_F());
		balls.push_back(ToPtr(new Sphere(center, 10)));
	}
	auto ballBVH = ToPtr(new BVH_Node(balls));
	mat4 tfmBalls(1.0f);
	tfmBalls = translate(tfmBalls, vec3(-100, 270, 395));
	tfmBalls = rotate(tfmBalls, Math::PI / 12, vec3(0, 1, 0));
	auto whiteMat = ToPtr(new Lambertian(OpTexture::ConstantTexture(vec3(0.73f, 0.73f, 0.73f))));
	auto tfmBallBVH = ToPtr(new Transform(tfmBalls, ballBVH, whiteMat));

	// Group
	auto group = ToPtr(new Group);
	(*group) << light
		<< moveSphere << sphere1 << sphere2 << noiseSphere << earth
		<< sphereBoundary << sphereVolume << airVolume
		<< boxBVH << tfmBallBVH;

	return ToPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene4(float ratioWH) {
	auto skyMat = ToPtr(new OpMaterial([](const HitRecord & rec)->bool {
		float t = 0.5f * (rec.vertex.pos.y + 1.0f);
		rgb c0 = rgb(0.2f);
		rgb c1 = c0 * rgb(0.75f, 0.5f, 0.375f);
		rgb lightColor = (1 - t) * c0 + t * c1;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	vector<Hitable::CPtr> balls;
	for (int i = -10; i <= 10; i+=2) {
		for (int j = -10; j <= 10; j+=2) {
			for (int k = -10; k <= 10; k+=2) {
				float q = pow(Math::Rand_F()*abs(i) + Math::Rand_F()*abs(j) + Math::Rand_F()*abs(k),2)/900;
				if (q < 0.09)
					continue;

				float p = Math::Rand_F();
				vec3 center = vec3(i, j, k) + (1.0f * vec3(Math::Rand_F(), Math::Rand_F(), Math::Rand_F())-0.5f);
				float radius = 0.5f + Math::Rand_F()*0.5f;
				rgb color = vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F());
				if (p < 0.2) {
					// glass
					auto mat = ToPtr(new Dielectric(1.1f + Math::Rand_F()*0.9f));
					auto sphere = ToPtr(new Sphere(center, radius, mat));
					balls.push_back(sphere);
				}
				else if (p < 0.4) {
					// inner empty glass
					auto mat = ToPtr(new Dielectric(1.1f + Math::Rand_F()*0.9f));
					auto sphereOut = ToPtr(new Sphere(center, radius, mat));
					auto sphereInner = ToPtr(new Sphere(center, radius*0.8f, mat));
					balls.push_back(sphereOut);
					balls.push_back(sphereInner);
				}
				else if (p < 0.8) {
					// Sphere Volume with Dielelctric Boundary
					auto sphereBoundary = ToPtr(new Sphere(center, radius, ToPtr(new Dielectric(1.1f + Math::Rand_F()*0.9f))));
					auto sphereVolume = ToPtr(new Volume(sphereBoundary, 0.2f, ToPtr(new Isotropic(color))));
					balls.push_back(sphereBoundary);
					balls.push_back(sphereVolume);
				}
				else if (p < 0.9) {
					// inner empty glass
					auto mat = ToPtr(new Dielectric(1.1f + Math::Rand_F()*0.9f));
					auto sphereOut = ToPtr(new Sphere(center, radius, mat));
					auto sphereInner = ToPtr(new Sphere(center, radius*0.8f, mat));
					// Light
					float lightness = Math::Rand_F()*3.0f + 1.0f;
					auto light = ToPtr(new Sphere(center, 0.75f*radius, ToPtr(new Light(lightness*vec3(vec3(color))))));
					balls.push_back(sphereOut);
					balls.push_back(sphereInner);
					balls.push_back(light);
				}
				else{
					// Light
					float lightness = Math::Rand_F()*3.0f + 1.0f;
					auto mat = ToPtr(new Light(lightness*vec3(vec3(color))));
					auto light = ToPtr(new Sphere(center, 0.7f*radius, mat));
					balls.push_back(light);
				}
			}
		}
	}
	auto ballsBVH = ToPtr(new BVH_Node(balls));

	vec3 origin(30, 30, 30);
	vec3 viewPoint(0, 0, 0);
	float fov = 37.0f;
	float lenR = 0.00f;
	//float distToFocus = 10.0f;
	auto camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, fov, lenR));
	
	auto scene = ToPtr(new Scene(ballsBVH, camera));
	return scene;
}

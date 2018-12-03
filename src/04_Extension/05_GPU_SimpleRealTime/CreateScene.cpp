#include "CreateScene.h"

#include <RayTracing/Model.h>
#include <RayTracing/Skybox.h>
#include <RayTracing/Sky.h>
#include <RayTracing/Volume.h>
#include <RayTracing/Isotropic.h>
#include <RayTracing/TriMesh.h>
#include <RayTracing/Light.h>
#include <RayTracing/BVH_Node.h>
#include <RayTracing/Group.h>
#include <RayTracing/Dielectric.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/ConstTexture.h>
#include <RayTracing/ImgTexture.h>
#include <RayTracing/TRayCamera.h>
#include <RayTracing/Transform.h>

#include <Utility/Math.h>
#include <Utility/Config.h>
#include <Utility/GStorage.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Defines.h"
#include <OpenGL/CommonDefine.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace Define;
using namespace glm;
using namespace std;

Scene::CPtr CreateScene0(float ratioWH) {
	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto rootPath = *config->GetStrPtr("RootPath");

	vector<string> skyboxImgPath(6);
	for (size_t i = 0; i < 6; i++)
		skyboxImgPath[i] = rootPath + str_Vec_Img_Skybox[i];
	auto sky = ToPtr(new Sky(ToCPtr(new Light(ToCPtr(new Skybox(skyboxImgPath))))));

	auto earthTex = ToCPtr(new ImgTexture(rootPath + str_Img_Earth, true));
	if (!earthTex->IsValid()) {
		printf("ERROR: earthTex[%s] is invalid.\n", (str_RootPath + str_Img_Earth).c_str());
		exit(1);
	}

	auto constTex = ToCPtr(new ConstTexture(rgb(0.5, 0.5, 0.5)));

	// Mesh
	// Mesh Square
	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 8) {
		vec3 pos(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2]);
		vec3 normal(data_SquareVertexPos[i + 3], data_SquareVertexPos[i + 4], data_SquareVertexPos[i + 5]);
		vec2 uv(data_SquareVertexPos[i + 6], data_SquareVertexPos[i + 7]);
		squareVertexs.push_back(Vertex(pos, normal, uv.x, uv.y));
	}
	auto square = ToPtr(new TriMesh(squareVertexs));
	if (!square->IsValid()) {
		printf("ERROR: square is invalid.\n");
		exit(1);
	}
	mat4 tfmBottom(1.0f);
	tfmBottom = translate(tfmBottom, vec3(0, -0.5, 0));
	tfmBottom = rotate(tfmBottom, -Math::PI / 2, vec3(1, 0, 0));
	tfmBottom = scale(tfmBottom, vec3(8));
	auto woodMat = ToCPtr(new Lambertian(ToPtr(new ImgTexture(rootPath + str_Img_Wood))));
	auto bottomWood = ToPtr(new Transform(tfmBottom, square, woodMat));

	Sphere::CPtr sphereTop = ToCPtr(new Sphere(vec3(0, 0.5, 0), 0.25, ToCPtr(new Metal(constTex, 0.2f))));
	Sphere::CPtr sphereBottom = ToCPtr(new Sphere(vec3(0, -100.5, -1), 100, ToCPtr(new Lambertian(constTex))));

	Sphere::CPtr sphereMid = ToCPtr(new Sphere(vec3(0, 0, 0), 0.5, ToCPtr(new Metal(vec3(1.0), 0.2))));

	auto dielectric = ToCPtr(new Dielectric(1.5));
	Sphere::CPtr sphereLeftOut = ToCPtr(new Sphere(vec3(-1.1, 0.01, 0), 0.5, dielectric));
	Sphere::CPtr sphereLeftIn = ToCPtr(new Sphere(vec3(-1.1, 0.01, 0), -0.45, dielectric));
	Group::Ptr group0 = ToPtr(new Group);
	(*group0) << sphereLeftOut << sphereLeftIn;

	Sphere::CPtr sphereRight = ToCPtr(new Sphere(vec3(1.1, 0, 0), 0.5, ToCPtr(new Lambertian(earthTex))));
	Group::Ptr group1 = ToPtr(new Group);

	const vec3 pos(0, 0, 1.2);
	const vec3 viewPoint(0, 0, 0);
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	(*group1) << bottomWood << sphereMid << group0 << sphereRight << sky;

	return ToCPtr(new Scene(group1, camera));
}

Scene::CPtr CreateScene1(float ratioWH) {
	auto group = ToPtr(new Group);
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Math::Rand_F();
			vec3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					auto mat = ToPtr(new Lambertian(vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F())));
					auto sphere = ToPtr(new Sphere(center, 0.2f, mat));
					group->push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToPtr(new Metal(vec3(0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F())), 0.5f*Math::Rand_F()));
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
	auto sphere4 = ToPtr(new Sphere(vec3(-6, 1, 0), 1.0, ToPtr(new Dielectric(2.5, vec3(0, 100, 0)))));

	(*group) << sphere0 << sphere1 << sphere2 << sphere3 << sphere4 << sphereBottom;

	vec3 origin(13, 2, 3);
	vec3 viewPoint(0, 0, 0);
	float fov = 20.0f;
	float lenR = 0.05;
	float distToFocus = 10.0f;
	RayCamera::Ptr camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, fov, lenR, distToFocus));

	return ToPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene2(float ratioWH) {
	// Mesh
	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 8) {
		vec3 pos(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2]);
		vec3 normal(data_SquareVertexPos[i + 3], data_SquareVertexPos[i + 4], data_SquareVertexPos[i + 5]);
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
	auto redMat = ToPtr(new Lambertian(vec3(0.65f, 0.05f, 0.05f)));
	auto greenMat = ToPtr(new Lambertian(vec3(0.12f, 0.45f, 0.15f)));
	auto grayMat = ToPtr(new Lambertian(vec3(0.73f, 0.73f, 0.73f)));
	auto lightMat = ToPtr(new Light(rgb(3.0f)));
	auto cubeMat = ToPtr(new Lambertian(rgb(1.0f)));
	auto volumnMat = ToPtr(new Isotropic(vec3(1.0f)));

	// Transform
	mat4 tfmRight(1.0f);
	tfmRight = translate(tfmRight, vec3(3, 0, 0));
	tfmRight = scale(tfmRight, vec3(6));
	tfmRight = rotate(tfmRight, -Math::PI / 2, vec3(0, 1, 0));
	auto redWall = ToPtr(new Transform(tfmRight, square, redMat));

	mat4 tfmLeft(1.0f);
	tfmLeft = translate(tfmLeft, vec3(-3, 0, 0));
	tfmLeft = scale(tfmLeft, vec3(6));
	tfmLeft = rotate(tfmLeft, Math::PI / 2, vec3(0, 1, 0));
	auto greenWall = ToPtr(new Transform(tfmLeft, square, greenMat));

	mat4 tfmBottom(1.0f);
	tfmBottom = translate(tfmBottom, vec3(0, -3, 0));
	tfmBottom = scale(tfmBottom, vec3(6));
	tfmBottom = rotate(tfmBottom, -Math::PI / 2, vec3(1, 0, 0));
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
	tfmCube1 = rotate(tfmCube1, -Math::PI / 12, vec3(0, 1, 0));
	auto cube1 = ToPtr(new Transform(tfmCube1, cube, cubeMat));

	mat4 tfmCube2(1.0f);
	tfmCube2 = translate(tfmCube2, vec3(-1.1, -1.5, -0.5));
	tfmCube2 = scale(tfmCube2, vec3(1.6, 3, 1.6));
	tfmCube2 = rotate(tfmCube2, Math::PI / 9, vec3(0, 1, 0));
	//tfmCube2 = scale(tfmCube2, vec3(4));
	auto volumn = ToPtr(new Volume(cube, 3.0f, volumnMat));
	auto cube2 = ToPtr(new Transform(tfmCube2, volumn));


	// Scene
	auto group = ToPtr(new Group);

	(*group) << greenWall << redWall << bottomWall << topWall << backWall << cube1/* << cube2*/ << light;

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
	vector<Hitable::CPtr> balls;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Math::Rand_F();
			vec3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					auto mat = ToCPtr(new Lambertian(vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F())));
					auto sphere = ToCPtr(new Sphere(center, 0.2f, mat));
					balls.push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToCPtr(new Metal(vec3(0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F())), 0.5f*Math::Rand_F()));
					auto sphere = ToCPtr(new Sphere(center, 0.2, mat));
					balls.push_back(sphere);
				}
				else {  // glass
					auto mat = ToCPtr(new Dielectric(1.5));
					auto sphere = ToCPtr(new Sphere(center, 0.2, mat));
					balls.push_back(sphere);
				}
			}
		}
	}
	auto bvhBalls = ToCPtr(new BVH_Node(balls));

	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto rootPath = *config->GetStrPtr("RootPath");
	auto earthTex = ToPtr(new ImgTexture(rootPath + str_Img_Earth, true));
	if (!earthTex->IsValid()) {
		printf("ERROR: earthTex[%s] is invalid.\n", (str_RootPath + str_Img_Earth).c_str());
		exit(1);
	}

	auto group = ToPtr(new Group);
	auto sphereBottom = ToCPtr(new Sphere(vec3(0, -1000, 0), 1000, ToCPtr(new Lambertian(vec3(0.5, 0.5, 0.5)))));
	auto sphere0 = ToCPtr(new Sphere(vec3(6, 1, 0), 1.0, ToCPtr(new Metal(vec3(0.7, 0.6, 0.5), 0.0))));
	auto sphere1 = ToCPtr(new Sphere(vec3(2, 1, 0), 1.0, ToCPtr(new Dielectric(1.5))));
	auto sphere2 = ToCPtr(new Sphere(vec3(2, 1, 0), -0.8, ToCPtr(new Dielectric(1.5))));
	auto sphere3 = ToCPtr(new Sphere(vec3(-2, 1, 0), 1.0, ToCPtr(new Lambertian(earthTex))));
	auto sphere4 = ToCPtr(new Sphere(vec3(-6, 1, 0), 1.0, ToCPtr(new Dielectric(2.5, vec3(0, 100, 0)))));

	(*group) << sphere0 << sphere1 << sphere2 << sphere3 << sphere4 << sphereBottom << bvhBalls;

	vec3 origin(13, 2, 3);
	vec3 viewPoint(0, 0, 0);
	float fov = 20.0f;
	float lenR = 0.05;
	float distToFocus = 10.0f;
	RayCamera::CPtr camera = ToCPtr(new RayCamera(origin, viewPoint, ratioWH, fov, lenR, distToFocus));

	return ToCPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene4(float ratioWH) {
	auto sky = ToCPtr(new Sphere(vec3(0, 0, 0), 1000, ToCPtr(new Light(vec3(0.01f)))));

	auto group = ToPtr(new Group);
	auto redLightMat = ToPtr(new Light(rgb(6.0f, 0, 0), 0.22f, 0.20f));
	auto greenLightMat = ToPtr(new Light(rgb(0, 6.0f, 0), 0.22f, 0.20f));
	auto blueLightMat = ToPtr(new Light(rgb(0, 0, 6.0f), 0.22f, 0.20f));
	auto whiteLightMat = ToPtr(new Light(rgb(6.0f), 0.22f, 0.20f));

	auto sphere00 = ToPtr(new Sphere(vec3(0, 1, 0), 1.0, ToPtr(new Dielectric(1.5))));
	auto sphere01 = ToPtr(new Sphere(vec3(0, 1, 0), -0.8, ToPtr(new Dielectric(1.5))));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -1000, 0), 1000.0f, ToCPtr(new Lambertian(rgb(0.8)))));
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

Scene::CPtr CreateScene5(float ratioWH) {
	auto sky = ToCPtr(new Sphere(vec3(0, 0, 0), 1000, ToCPtr(new Light(vec3(0.0f)))));

	auto group = ToPtr(new Group);

	auto sphere00 = ToCPtr(new Sphere(vec3(0, 1, 0), 1.0, ToCPtr(new Dielectric(1.5))));
	auto sphere01 = ToCPtr(new Sphere(vec3(0, 1, 0), -0.8, ToCPtr(new Dielectric(1.5))));
	auto sphere1 = ToCPtr(new Sphere(vec3(0, -1000, 0), 1000.0f, ToCPtr(new Lambertian(rgb(0.8)))));

	vector<Vertex> vertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 3)
		vertexs.push_back(Vertex(vec3(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2])));

	auto lightMat = ToCPtr(new Light(vec3(4)));
	auto triMesh = ToCPtr(new TriMesh(vertexs, lightMat));
	if (!triMesh->IsValid()) {
		printf("ERROR: triMesh is invalid.\n");
		exit(1);
	}

	(*group) << sphere00 << sphere01 << sphere1 << triMesh << sky;

	float t0 = 0.0f;
	float t1 = 1.0f;
	vec3 origin(0, 1, 5);
	vec3 viewPoint(0, 1, 0);
	float fov = 45.0f;
	float lenR = 0.05f;
	float distToFocus = 5.0f;
	TRayCamera::CPtr camera = ToCPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));

	return ToCPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene6(float ratioWH) {
	auto sky = ToCPtr(new Sphere(vec3(0, 0, 0), 1000, ToCPtr(new Light(vec3(0.0f)))));

	// Mesh
	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 8) {
		vec3 pos(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2]);
		vec3 normal(data_SquareVertexPos[i + 3], data_SquareVertexPos[i + 4], data_SquareVertexPos[i + 5]);
		squareVertexs.push_back(Vertex(pos, normal));
	}
	auto square = ToCPtr(new TriMesh(squareVertexs));
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
	auto cube = ToCPtr(new TriMesh(cubeVertexs));
	if (!cube->IsValid()) {
		printf("ERROR: cube is invalid.\n");
		exit(1);
	}

	// Mat 
	auto redMat = ToCPtr(new Lambertian(vec3(0.65f, 0.05f, 0.05f)));
	auto greenMat = ToCPtr(new Lambertian(vec3(0.12f, 0.45f, 0.15f)));
	auto grayMat = ToCPtr(new Lambertian(vec3(0.73f, 0.73f, 0.73f)));
	auto lightMat = ToCPtr(new Light(rgb(4.0f)));
	auto cubeMat = ToCPtr(new Lambertian(rgb(1.0f)));

	// Transform
	mat4 tfmRight(1.0f);
	tfmRight = translate(tfmRight, vec3(3, 0, 0));
	tfmRight = scale(tfmRight, vec3(6));
	tfmRight = rotate(tfmRight, -Math::PI / 2, vec3(0, 1, 0));
	auto redWall = ToCPtr(new Transform(tfmRight, square, redMat));

	mat4 tfmLeft(1.0f);
	tfmLeft = translate(tfmLeft, vec3(-3, 0, 0));
	tfmLeft = scale(tfmLeft, vec3(6));
	tfmLeft = rotate(tfmLeft, Math::PI / 2, vec3(0, 1, 0));
	auto greenWall = ToCPtr(new Transform(tfmLeft, square, greenMat));

	mat4 tfmBottom(1.0f);
	tfmBottom = translate(tfmBottom, vec3(0, -3, 0));
	tfmBottom = scale(tfmBottom, vec3(6));
	tfmBottom = rotate(tfmBottom, -Math::PI / 2, vec3(1, 0, 0));
	auto bottomWall = ToCPtr(new Transform(tfmBottom, square, grayMat));

	mat4 tfmTop(1.0f);
	tfmTop = translate(tfmTop, vec3(0, 3, 0));
	tfmTop = scale(tfmTop, vec3(6));
	tfmTop = rotate(tfmTop, Math::PI / 2, vec3(1, 0, 0));
	auto topWall = ToCPtr(new Transform(tfmTop, square, grayMat));

	mat4 tfmBack(1.0f);
	tfmBack = translate(tfmBack, vec3(0, 0, -3));
	tfmBack = scale(tfmBack, vec3(6));
	auto backWall = ToCPtr(new Transform(tfmBack, square, grayMat));

	mat4 tfmLight(1.0f);
	tfmLight = translate(tfmLight, vec3(0, 2.999, 0));
	tfmLight = scale(tfmLight, vec3(4));
	tfmLight = rotate(tfmLight, Math::PI / 2, vec3(1, 0, 0));
	auto light = ToCPtr(new Transform(tfmLight, square, lightMat));

	mat4 tfmCube1(1.0f);
	tfmCube1 = translate(tfmCube1, vec3(1.1, -2.2, 1.5));
	tfmCube1 = scale(tfmCube1, vec3(1.6));
	tfmCube1 = rotate(tfmCube1, -Math::PI / 12, vec3(0, 1, 0));
	auto cube1 = ToCPtr(new Transform(tfmCube1, cube, cubeMat));

	mat4 tfmCube2(1.0f);
	tfmCube2 = translate(tfmCube2, vec3(-1.1, -1.49, -0.5));
	tfmCube2 = scale(tfmCube2, vec3(1.6, 3, 1.6));
	tfmCube2 = rotate(tfmCube2, Math::PI / 9, vec3(0, 1, 0));
	auto cube2 = ToPtr(new Transform(tfmCube2, cube, ToCPtr(new Dielectric(1.5f))));
	auto volume = ToPtr(new Volume(cube2, 160.5f, ToPtr(new Isotropic(vec3(0.2, 0.5, 0.9f)))));


	// Scene
	auto group = ToPtr(new Group);

	(*group) << redWall << greenWall << bottomWall << topWall << backWall << cube1 << cube2 << volume << light << sky;

	// Camera
	float t0 = 0.0f;
	float t1 = 1.0f;
	vec3 origin(0, 0, 10);
	vec3 viewPoint(0, 0, 0);
	float fov = 45.0f;
	float lenR = 0.05f;
	float distToFocus = 8.0f;
	TRayCamera::CPtr camera = ToCPtr(new TRayCamera(origin, viewPoint, ratioWH, t0, t1, fov, lenR, distToFocus));

	return ToCPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene7(float ratioWH) {
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
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 8) {
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

	auto sky = ToCPtr(new Sphere(vec3(0, 0, 0), 10000, ToCPtr(new Light(vec3(0.0f)))));

	// Ground
	int nb = 20;
	auto groundMat = ToPtr(new Lambertian(vec3(0.48f, 0.83f, 0.53f)));
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
	lightTfm = translate(lightTfm, vec3((123 + 423) / 2, 554, (147 + 412) / 2));
	lightTfm = rotate(lightTfm, Math::PI / 2, vec3(1, 0, 0));
	lightTfm = scale(lightTfm, vec3(423 - 123, 412 - 127, 1.00));
	auto light = ToPtr(new Transform(lightTfm, square, lightMat));

	// Move Sphere
	//vec3 center(400, 400, 200);
	//auto moveSphereMat = ToPtr(new Lambertian(vec3(0.7, 0.3, 0.1)));
	//auto moveSphere = ToPtr(new MoveSphere(t0, t1, center, center + vec3(30, 0, 0), 50, moveSphereMat));

	// Dielectric, Metal
	auto sphere1 = ToPtr(new Sphere(vec3(260, 150, 45), 50, ToPtr(new Dielectric(1.5f))));
	auto sphere2 = ToPtr(new Sphere(vec3(0, 150, 145), 50, ToPtr(new Metal(vec3(0.8f, 0.8f, 0.9f), 10.0f))));

	// Sphere Volume with Dielelctric Boundary
	auto gvSphere = ToPtr(new Sphere(vec3(0), 1, ToPtr(new Dielectric(1.5f/*, vec3(0.01, 1, 0.01)/*/))));

	mat4 tfm0 = mat4(1.0);
	tfm0 = translate(tfm0, vec3(360, 150, 145));
	tfm0 = scale(tfm0, vec3(0.95 * 70));
	auto volumeBoundary = ToCPtr(new Transform(tfm0, gvSphere));
	mat4 tfm1 = mat4(1.0);
	tfm1 = translate(tfm1, vec3(360, 150, 145));
	tfm1 = scale(tfm1, vec3(70));
	auto glass = ToCPtr(new Transform(tfm1, gvSphere));
	auto sphereVolume = ToPtr(new Volume(volumeBoundary, 0.2f, ToPtr(new Isotropic(rgb(0.2f, 0.4, 0.9f)))));

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
	//auto noiseMat = ToPtr(new Lambertian(OpTexture::NoiseTexture(0, vec3(1), 0.1)));
	//auto noiseSphere = ToPtr(new Sphere(vec3(220, 280, 300), 80, noiseMat));

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
	auto whiteMat = ToPtr(new Lambertian(vec3(0.73f, 0.73f, 0.73f)));
	auto tfmBallBVH = ToPtr(new Transform(tfmBalls, ballBVH, whiteMat));

	// Group
	auto group = ToPtr(new Group);
	(*group) << light
		/*<< moveSphere*/ << sphere1 << sphere2 /*<< noiseSphere*/ << earth
		<< glass << sphereVolume << airVolume
		<< boxBVH << tfmBallBVH << sky;

	return ToPtr(new Scene(group, camera));
}

Scene::CPtr CreateScene8(float ratioWH) {
	auto config = *GStorage<Ptr<Config>>::GetInstance()->GetPtr(str_MainConfig);
	auto rootPath = *config->GetStrPtr("RootPath");
	vector<string> skyboxImgPath(6);
	for (size_t i = 0; i < 6; i++)
		skyboxImgPath[i] = rootPath + str_Vec_Img_Skybox[i];
	auto sky = ToPtr(new Sky(ToCPtr(new Light(ToCPtr(new Skybox(skyboxImgPath))))));
	//auto sky = ToPtr(new Sky(ToCPtr(new Light(ToCPtr(new ImgTexture(rootPath + str_Img_SkySphere))))));

	// Mesh
	// Mesh Square
	vector<Vertex> squareVertexs;
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 8) {
		vec3 pos(data_SquareVertexPos[i], data_SquareVertexPos[i + 1], data_SquareVertexPos[i + 2]);
		vec3 normal(data_SquareVertexPos[i + 3], data_SquareVertexPos[i + 4], data_SquareVertexPos[i + 5]);
		vec2 uv(data_SquareVertexPos[i + 6], data_SquareVertexPos[i + 7]);
		squareVertexs.push_back(Vertex(pos, normal, uv.x, uv.y));
	}
	auto square = ToPtr(new TriMesh(squareVertexs));
	if (!square->IsValid()) {
		printf("ERROR: square is invalid.\n");
		exit(1);
	}
	mat4 tfmBottom(1.0f);
	tfmBottom = translate(tfmBottom, vec3(0, -1, 0));
	tfmBottom = rotate(tfmBottom, -Math::PI / 2, vec3(1, 0, 0));
	tfmBottom = scale(tfmBottom, vec3(8));
	auto woodMat = ToCPtr(new Lambertian(ToPtr(new ImgTexture(rootPath + str_Img_Wood))));
	auto bottomWood = ToPtr(new Transform(tfmBottom, square, woodMat));

	auto sphere0 = ToPtr(new Sphere(vec3(0, -0.25, 0), 0.75, ToCPtr(new Metal(vec3(1.0)))));
	//auto sphere2 = ToPtr(new Sphere(vec3(0, 0, 0), 1, ToCPtr(new Dielectric(1.5f))));

	//------------ bunny
	auto bunnyModel = ToCPtr(new Model(rootPath + str_Obj_Bunny));

	mat4 bunnyInstanceMat(1.0);
	bunnyInstanceMat = translate(bunnyInstanceMat, vec3(1.2, -0.25, 1.5));
	bunnyInstanceMat = scale(bunnyInstanceMat, vec3(1.2));
	bunnyInstanceMat = rotate(bunnyInstanceMat, - Math::PI / 6, vec3(0, 1, 0));

	mat4 bunnyModelNormMat(1.0f);
	bunnyModelNormMat = scale(bunnyModelNormMat, vec3(1.0f / bunnyModel->GetBoundingBox().GetRadius()));
	bunnyModelNormMat = translate(bunnyModelNormMat, -bunnyModel->GetBoundingBox().GetCenter());

	mat4 bunnyGlassMat = bunnyInstanceMat;
	bunnyGlassMat *= bunnyModelNormMat;
	auto bunnyGlass = ToCPtr(new Transform(bunnyGlassMat, bunnyModel, ToCPtr(new Dielectric(1.5f))));

	mat4 bunnyVolumeMat = bunnyInstanceMat;
	bunnyVolumeMat = scale(bunnyVolumeMat, vec3(0.99f));
	bunnyVolumeMat *= bunnyModelNormMat;
	auto bunnyVolumeBoundary = ToCPtr(new Transform(bunnyVolumeMat, bunnyModel));
	auto bunnyVolume = ToCPtr(new Volume(bunnyVolumeBoundary, 8.0f, ToCPtr(new Isotropic(vec3(1.0f, 0.684f, 0.785f)))));
	auto bunny = ToPtr(new Group);
	(*bunny) << bunnyGlass << bunnyVolume;

	//------------ dragon
	auto dragonModel = ToCPtr(new Model(rootPath + str_Obj_Dragon));

	mat4 dragonModelNormMat(1.0);
	dragonModelNormMat = scale(dragonModelNormMat, vec3(1.0f/dragonModel->GetBoundingBox().GetRadius()));
	dragonModelNormMat = translate(dragonModelNormMat, - dragonModel->GetBoundingBox().GetCenter());

	mat4 dragonInstanceMat(1.0);
	dragonInstanceMat = translate(dragonInstanceMat, vec3(-1.2, -0.25, 1.6));
	dragonInstanceMat = rotate(dragonInstanceMat, Math::PI / 6, vec3(0, 1, 0));
	dragonInstanceMat = scale(dragonInstanceMat, vec3(1.2));

	mat4 tfmDragonGlass = dragonInstanceMat;
	tfmDragonGlass *= dragonModelNormMat;
	auto dragonGlass = ToCPtr(new Transform(tfmDragonGlass, dragonModel, ToCPtr(new Dielectric(1.5f))));

	mat4 tfmDragonVolumeModel = dragonInstanceMat;
	tfmDragonVolumeModel = scale(tfmDragonVolumeModel, vec3(0.99));
	tfmDragonVolumeModel *= dragonModelNormMat;
	auto dragonVolumeModel = ToCPtr(new Transform(tfmDragonVolumeModel, dragonModel));

	auto dragonVolume = ToPtr(new Volume(dragonVolumeModel, 16.0f, ToPtr(new Isotropic(rgb(0, 0.644, 0.855f)))));
	auto dragon = ToPtr(new Group);
	(*dragon) << dragonGlass << dragonVolume;

	auto group = ToPtr(new Group);
	auto group1 = ToPtr(new Group);
	(*group) << sphere0 << bottomWood << bunny << dragon << sky;// << sphere1 << sphere2;

	vec3 origin(0, 0.75, 4);
	vec3 viewPoint(0, 0, 0.5);
	float fov = 60.0f;
	float lenR = 0.00f;
	//float distToFocus = 10.0f;
	auto camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, fov, lenR));

	auto scene = ToPtr(new Scene(group, camera));
	return scene;
}

#include "CreateScene.h"

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
	auto earthTex = ToCPtr(new ImgTexture(rootPath + str_Img_Earth, true));
	if (!earthTex->IsValid()) {
		printf("ERROR: earthTex[%s] is invalid.\n", (str_RootPath + str_Img_Earth).c_str());
		exit(1);
	}

	auto constTex = ToCPtr(new ConstTexture(rgb(0.5, 0.5, 0.5)));
	Sphere::CPtr sphereTop = ToCPtr(new Sphere(vec3(0, 1, -1), 0.25, ToCPtr(new Metal(constTex, 0.2f))));
	Sphere::CPtr sphereBottom = ToCPtr(new Sphere(vec3(0, -100.5, -1), 100, ToCPtr(new Lambertian(constTex))));

	Sphere::CPtr sphereMid = ToCPtr(new Sphere(vec3(0, 0, -1), 0.5, ToCPtr(new Metal(earthTex, 0.2))));

	auto dielectric = ToCPtr(new Dielectric(1.5));
	Sphere::CPtr sphereLeftOut = ToCPtr(new Sphere(vec3(-1, 0, -1), 0.5, dielectric));
	Sphere::CPtr sphereLeftIn = ToCPtr(new Sphere(vec3(-1, 0, -1), -0.45, dielectric));
	Group::Ptr group0 = ToPtr(new Group);
	(*group0) << sphereLeftOut << sphereLeftIn;

	Sphere::CPtr sphereRight = ToCPtr(new Sphere(vec3(1, 0, -1), 0.5, ToCPtr(new Lambertian(rgb(0.1, 0.2, 0.5)))));
	Group::Ptr group1 = ToPtr(new Group);

	const vec3 pos(0, 0, 0);
	const vec3 viewPoint(0, 0, -1);
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	(*group1) << sphereTop << sphereBottom << sphereMid << group0 << sphereRight;

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
	Sphere::CPtr sphereBottom = ToCPtr(new Sphere(vec3(0, -100.5, -1), 100, ToCPtr(new Lambertian(rgb(0.5, 0.5, 0.5)))));
	Sphere::CPtr sphereMid = ToCPtr(new Sphere(vec3(0, 0, -1), 0.5, ToCPtr(new Lambertian(rgb(0.8, 0.8, 0)))));
	Sphere::CPtr sphereLeftOut = ToCPtr(new Sphere(vec3(-1, 0, -1), 0.5, ToCPtr(new Dielectric(1.5))));
	Sphere::CPtr sphereLeftIn = ToCPtr(new Sphere(vec3(-1, 0, -1), -0.45, ToCPtr(new Dielectric(1.5))));
	Sphere::CPtr sphereRight = ToCPtr(new Sphere(vec3(1, 0, -1), 0.5, ToCPtr(new Metal(rgb(0.1, 0.2, 0.5), 0.0))));
	vector<Hitable::CPtr> balls;
	balls.push_back(sphereMid);
	balls.push_back(sphereBottom);
	BVH_Node::CPtr bvhBalls = ToCPtr(new BVH_Node(balls));

	Group::Ptr group0 = ToPtr(new Group);
	Group::Ptr group1 = ToPtr(new Group);
	(*group1) << sphereLeftOut << sphereLeftIn;

	const vec3 pos(0, 0, 0);
	const vec3 viewPoint(0, 0, -1);
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	(*group0) << sphereBottom << sphereMid << group1 << sphereRight;

	return ToPtr(new Scene(bvhBalls, camera));
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
	for (size_t i = 0; i < sizeof(data_SquareVertexPos) / sizeof(float); i += 6) {
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
	tfmLight = scale(tfmLight, vec3(3));
	tfmLight = rotate(tfmLight, Math::PI / 2, vec3(1, 0, 0));
	auto light = ToCPtr(new Transform(tfmLight, square, lightMat));

	mat4 tfmCube1(1.0f);
	tfmCube1 = translate(tfmCube1, vec3(1.1, -2.2, 1.5));
	tfmCube1 = scale(tfmCube1, vec3(1.6));
	tfmCube1 = rotate(tfmCube1, -Math::PI / 12, vec3(0, 1, 0));
	auto cube1 = ToCPtr(new Transform(tfmCube1, cube, cubeMat));

	mat4 tfmCube2(1.0f);
	tfmCube2 = translate(tfmCube2, vec3(-1.1, -1, -0.5));
	tfmCube2 = scale(tfmCube2, vec3(1.6, 4, 1.6));
	tfmCube2 = rotate(tfmCube2, Math::PI / 9, vec3(0, 1, 0));
	auto cube2 = ToCPtr(new Transform(tfmCube2, cube, cubeMat));


	// Scene
	auto group = ToPtr(new Group);

	(*group) << greenWall << redWall << bottomWall << topWall << backWall << cube1 << cube2 << light << sky;

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
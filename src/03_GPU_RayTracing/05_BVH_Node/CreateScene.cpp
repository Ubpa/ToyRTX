#include "CreateScene.h"

#include <CppUtil/RTX/BVH_Node.h>
#include <CppUtil/RTX/Group.h>
#include <CppUtil/RTX/Dielectric.h>
#include <CppUtil/RTX/Metal.h>
#include <CppUtil/RTX/Lambertian.h>
#include <CppUtil/RTX/Sphere.h>
#include <CppUtil/RTX/ImgTexture.h>
#include <CppUtil/RTX/TRayCamera.h>

#include <CppUtil/Basic/Math.h>

#include "Defines.h"
#include <CppUtil/OpenGL/CommonDefine.h>
 
using namespace RTX;
using namespace CppUtil::Basic;
using namespace Define;
using namespace glm;
using namespace std;

Scene::CPtr CreateScene0(float ratioWH){
	Sphere::Ptr sphereBottom = ToPtr(new Sphere(vec3(0, -100.5, -1), 100, ToPtr(new Lambertian(rgb(0.5, 0.5, 0.5)))));
	Sphere::Ptr sphereMid = ToPtr(new Sphere(vec3(0, 0, -1), 0.5, ToPtr(new Lambertian(rgb(0.8, 0.8, 0)))));
	Sphere::Ptr sphereLeftOut = ToPtr(new Sphere(vec3(-1, 0, -1), 0.5, ToPtr(new Dielectric(1.5))));
	Sphere::Ptr sphereLeftIn = ToPtr(new Sphere(vec3(-1, 0, -1), -0.45, ToPtr(new Dielectric(1.5))));
	Sphere::Ptr sphereRight = ToPtr(new Sphere(vec3(1, 0, -1), 0.5, ToPtr(new Metal(rgb(0.1, 0.2, 0.5), 0.0))));
	Group::Ptr group0 = ToPtr(new Group);
	Group::Ptr group1 = ToPtr(new Group);
	(*group1) << sphereLeftOut << sphereLeftIn;

	const vec3 pos(0, 0, 0);
	const vec3 viewPoint(0, 0, -1);
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	(*group0) << sphereBottom << sphereMid << group1 << sphereRight;

	return ToCPtr(new Scene(group0, camera));
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

	auto group = ToPtr(new Group);
	auto sphereBottom = ToCPtr(new Sphere(vec3(0, -1000, 0), 1000, ToCPtr(new Lambertian(vec3(0.5, 0.5, 0.5)))));
	auto sphere0 = ToCPtr(new Sphere(vec3(6, 1, 0), 1.0, ToCPtr(new Metal(vec3(0.7, 0.6, 0.5), 0.0))));
	auto sphere1 = ToCPtr(new Sphere(vec3(2, 1, 0), 1.0, ToCPtr(new Dielectric(1.5))));
	auto sphere2 = ToCPtr(new Sphere(vec3(2, 1, 0), -0.8, ToCPtr(new Dielectric(1.5))));
	auto sphere3 = ToCPtr(new Sphere(vec3(-2, 1, 0), 1.0, ToCPtr(new Lambertian(vec3(0.4, 0.2, 0.1)))));
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

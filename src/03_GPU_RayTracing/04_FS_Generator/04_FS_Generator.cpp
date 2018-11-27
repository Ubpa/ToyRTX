#include "Defines.h"

#include <RayTracing/FS_Generator.h>
#include <RayTracing/ConstTexture.h>
#include <RayTracing/Dielectric.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/Group.h>
#include <RayTracing/TexWindow.h>
#include <RayTracing/TRayCamera.h>

#include <OpenGL/VAO.h>
#include <OpenGL/FBO.h>
#include <OpenGL/CommonDefine.h>

#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/Timer.h>
#include <Utility/Math.h>
#include <Utility/File.h>

#include <glm/glm.hpp>

using namespace RayTracing;
using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
using namespace glm;
using namespace std;

int main(int argc, char ** argv) {
	TexWindow texWindow(str_WindowTitle);
	if (!texWindow.IsValid()) {
		printf("ERROR: Texture Window Create Fail.\n");
		return 1;
	}
	string rootPath = texWindow.GetRootPath();
	int width = texWindow.GetWidth();
	int height = texWindow.GetHeight();

	//------------ Ä£ÐÍ . Screen
	VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });
	
	//------------ Scene
	Sphere::Ptr sphereBottom = ToPtr(new Sphere(vec3(0, -100.5, -1), 100, ToPtr(new Lambertian(rgb(0.5, 0.5, 0.5)))));
	Sphere::Ptr sphereMid = ToPtr(new Sphere(vec3(0, 0, -1), 0.5, ToPtr(new Lambertian(rgb(0.8, 0.8, 0)))));
	Sphere::Ptr sphereLeftOut = ToPtr(new Sphere(vec3(-1, 0, -1), 0.5, ToPtr(new Dielectric(1.5))));
	Sphere::Ptr sphereLeftIn = ToPtr(new Sphere(vec3(-1, 0, -1), -0.45, ToPtr(new Dielectric(1.5))));
	Sphere::Ptr sphereRight = ToPtr(new Sphere(vec3(1, 0, -1), 0.5, ToPtr(new Metal(rgb(0.1,0.2,0.5), 0.0))));
	Group::Ptr group = ToPtr(new Group);
	(*group) << sphereBottom << sphereMid << sphereLeftOut << sphereLeftIn << sphereRight;

	//------------ RayTracing Basic Shader
	string RTX_vs = rootPath + str_RTX_vs;
	FS_Generator fsGenerator(group);
	string RTX_fs_src = fsGenerator.BuildFS();
	string RTX_fs = rootPath + str_RTX_fs;
	File RTX_fs_file(RTX_fs, File::WRITE);
	RTX_fs_file.Printf("%s", RTX_fs_src.c_str());
	RTX_fs_file.Close();
	Shader RTX_Shader(RTX_vs, RTX_fs);
	if (!RTX_Shader.IsValid()) {
		printf("ERROR: RTX_Shader load fail.\n");
		return 1;
	}
	const float RayNumMax = 10000.0f;
	const vec3 pos(0, 0, 0);
	const vec3 viewPoint(0, 0, -1);
	const float ratioWH = width / height;
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	RTX_Shader.SetInt("origin_curRayNum", 0);
	RTX_Shader.SetInt("dir_tMax", 1);
	RTX_Shader.SetInt("color_time", 2);
	RTX_Shader.SetInt("rayTracingRst", 3);
	RTX_Shader.SetFloat("RayNumMax", RayNumMax);
	RTX_Shader.SetVec3f("camera.pos", camera->GetPos());
	RTX_Shader.SetVec3f("camera.BL_Corner", camera->GetBL_Corner());
	RTX_Shader.SetVec3f("camera.horizontal", camera->GetHorizontal());
	RTX_Shader.SetVec3f("camera.vertical", camera->GetVertical());
	RTX_Shader.SetVec3f("camera.right", camera->GetRight());
	RTX_Shader.SetVec3f("camera.up", camera->GetUp());
	RTX_Shader.SetVec3f("camera.front", camera->GetFront());
	RTX_Shader.SetFloat("camera.lenR", camera->GetLenR());
	RTX_Shader.SetFloat("camera.t0", camera->GetT1());
	RTX_Shader.SetFloat("camera.t1", camera->GetT0());

	//------------ RayTracing FBO 
	bool curReadFBO = false;
	bool curWriteFBO = !curReadFBO;
	FBO FBO_RayTracing[2] = {
		FBO(width, height, FBO::ENUM_TYPE_RAYTRACING),
		FBO(width, height, FBO::ENUM_TYPE_RAYTRACING),
	};

	//------------ ²Ù×÷
	Timer timer;
	timer.Start();
	auto rayTracingOp = ToPtr(new LambdaOp([&]() {
		size_t loopNum = static_cast<size_t>(glm::max(texWindow.GetScale(),1.0));
		for (size_t i = 0; i < loopNum; i++) {
			FBO_RayTracing[curWriteFBO].Use();
			FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
			FBO_RayTracing[curReadFBO].GetColorTexture(1).Use(1);
			FBO_RayTracing[curReadFBO].GetColorTexture(2).Use(2);
			FBO_RayTracing[curReadFBO].GetColorTexture(3).Use(3);
			RTX_Shader.SetFloat("rdSeed[0]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[1]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[2]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[3]", Math::Rand_F());
			VAO_Screen.Draw(RTX_Shader);

			curReadFBO = curWriteFBO;
			curWriteFBO = !curReadFBO;
		}
		texWindow.SetTex(FBO_RayTracing[curReadFBO].GetColorTexture(3));

		static size_t allLoopNum = 0;
		allLoopNum += loopNum;
		double wholeTime = timer.GetWholeTime();
		double speed = allLoopNum / wholeTime;
		printf("\rINFO: curLoopNum:%u, allLoopNum:%u, speed %.2f / s, used time: %.2f s     ",
			loopNum, allLoopNum, speed, wholeTime);
	}));
	
	bool success = texWindow.Run(rayTracingOp);
	return success ? 0 : 1;
}
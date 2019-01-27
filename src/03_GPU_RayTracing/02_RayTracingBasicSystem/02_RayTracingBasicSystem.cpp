#include "Defines.h"

#include <CppUtil/RTX/TexWindow.h>
#include <CppUtil/RTX/TRayCamera.h>

#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/FBO.h>
#include <CppUtil/OpenGL/CommonDefine.h>

#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/Timer.h>
#include <CppUtil/Basic/Math.h>

#include <glm/glm.hpp>
 
using namespace RTX;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace Define;
using namespace glm;
using namespace std;

int main(int argc, char ** argv) {
	printf("Need old version code to run this project\n");
	return 1;

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
	 
	//------------ RTX Basic Shader
	string RTXBasicSystem_vs = rootPath + str_RTXBasicSystem_vs;
	string RTXBasicSystem_fs = rootPath + str_RTXBasicSystem_fs;
	Shader RTXBasicSystemShader(RTXBasicSystem_vs, RTXBasicSystem_fs);
	if (!RTXBasicSystemShader.IsValid()) {
		printf("ERROR: RTXBasicSystemShader load fail.\n");
		return 1;
	}
	const float RayNumMax = 10000.0f;
	const vec3 pos(0, 0, 0);
	const vec3 viewPoint(0, 0, -1);
	const float ratioWH = width / height;
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	RTXBasicSystemShader.SetInt("origin_curRayNum", 0);
	RTXBasicSystemShader.SetInt("dir_tMax", 1);
	RTXBasicSystemShader.SetInt("color_time", 2);
	RTXBasicSystemShader.SetInt("rayTracingRst", 3);
	RTXBasicSystemShader.SetFloat("RayNumMax", RayNumMax);
	RTXBasicSystemShader.SetVec3f("camera.pos", camera->GetPos());
	RTXBasicSystemShader.SetVec3f("camera.BL_Corner", camera->GetBL_Corner());
	RTXBasicSystemShader.SetVec3f("camera.horizontal", camera->GetHorizontal());
	RTXBasicSystemShader.SetVec3f("camera.vertical", camera->GetVertical());
	RTXBasicSystemShader.SetVec3f("camera.right", camera->GetRight());
	RTXBasicSystemShader.SetVec3f("camera.up", camera->GetUp());
	RTXBasicSystemShader.SetVec3f("camera.front", camera->GetFront());
	RTXBasicSystemShader.SetFloat("camera.lenR", camera->GetLenR());
	RTXBasicSystemShader.SetFloat("camera.t0", camera->GetT1());
	RTXBasicSystemShader.SetFloat("camera.t1", camera->GetT0());

	//------------ RTX FBO 
	bool curReadFBO = false;
	bool curWriteFBO = !curReadFBO;
	FBO FBO_RayTracing[2] = {
		FBO(width, height, FBO::ENUM_TYPE_RAYTRACING),
		FBO(width, height, FBO::ENUM_TYPE_RAYTRACING),
	};

	//------------ ²Ù×÷
	Timer timer;
	timer.Start();
	auto RTXOp = ToPtr(new LambdaOp([&]() {
		size_t loopNum = static_cast<size_t>(glm::max(texWindow.GetScale(),1.0));
		for (size_t i = 0; i < loopNum; i++) {
			FBO_RayTracing[curWriteFBO].Use();
			FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
			FBO_RayTracing[curReadFBO].GetColorTexture(1).Use(1);
			FBO_RayTracing[curReadFBO].GetColorTexture(2).Use(2);
			FBO_RayTracing[curReadFBO].GetColorTexture(3).Use(3);
			RTXBasicSystemShader.SetFloat("rdSeed[0]", Math::Rand_F());
			RTXBasicSystemShader.SetFloat("rdSeed[1]", Math::Rand_F());
			RTXBasicSystemShader.SetFloat("rdSeed[2]", Math::Rand_F());
			RTXBasicSystemShader.SetFloat("rdSeed[3]", Math::Rand_F());
			VAO_Screen.Draw(RTXBasicSystemShader);

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
	
	bool success = texWindow.Run(RTXOp);
	return success ? 0 : 1;
}
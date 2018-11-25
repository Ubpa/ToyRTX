#include "Defines.h"

#include <RayTracing/TexWindow.h>

#include <OpenGL/VAO.h>
#include <OpenGL/FBO.h>
#include <OpenGL/CommonDefine.h>

#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/Timer.h>

#include <glm/glm.hpp>
 
using namespace RayTracing;
using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
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

	//------------ RayTracing Basic Shader
	string rayTracingBasic_vs = rootPath + str_RayTracingBasic_vs;
	string rayTracingBasic_fs = rootPath + str_RayTracingBasic_fs;
	Shader rayTracingBasicShader(rayTracingBasic_vs, rayTracingBasic_fs);
	if (!rayTracingBasicShader.IsValid()) {
		printf("ERROR: rayTracingBasicShader load fail.\n");
		return 1;
	} 
	const float RayNumMax = 3000.0f;
	rayTracingBasicShader.SetInt("origin_curRayNum", 0);
	rayTracingBasicShader.SetInt("dir_tMax", 1);
	rayTracingBasicShader.SetInt("color_time", 2);
	rayTracingBasicShader.SetInt("rayTracingRst", 3);
	rayTracingBasicShader.SetFloat("RayNumMax", RayNumMax);
	 
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
			FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
			FBO_RayTracing[curReadFBO].GetColorTexture(1).Use(1);
			FBO_RayTracing[curReadFBO].GetColorTexture(2).Use(2);
			FBO_RayTracing[curReadFBO].GetColorTexture(3).Use(3);
			FBO_RayTracing[curWriteFBO].Use();
			VAO_Screen.Draw(rayTracingBasicShader);

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
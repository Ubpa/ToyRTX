#include "Defines.h"

#include <RayTracing/TexWindow.h>
#include <RayTracing/TRayCamera.h>
#include <OpenGL/VAO.h>
#include <OpenGL/FBO.h>
#include <OpenGL/CommonDefine.h>

#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/Timer.h>
#include <Utility/Math.h>

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

	//------------ RayTracing Basic Shader
	string material_vs = rootPath + str_Material_vs;
	string material_fs = rootPath + str_Material_fs;
	Shader materialShader(material_vs, material_fs);
	if (!materialShader.IsValid()) {
		printf("ERROR: materialShader load fail.\n");
		return 1;
	}
	const float RayNumMax = 10000.0f;
	const vec3 pos(0, 0, 0);
	const vec3 viewPoint(0, 0, -1);
	const float ratioWH = width / height;
	const float fov = 90.0f;
	auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	materialShader.SetInt("origin_curRayNum", 0);
	materialShader.SetInt("dir_tMax", 1);
	materialShader.SetInt("color_time", 2);
	materialShader.SetInt("rayTracingRst", 3);
	materialShader.SetFloat("RayNumMax", RayNumMax);
	materialShader.SetVec3f("camera.pos", camera->GetPos());
	materialShader.SetVec3f("camera.BL_Corner", camera->GetBL_Corner());
	materialShader.SetVec3f("camera.horizontal", camera->GetHorizontal());
	materialShader.SetVec3f("camera.vertical", camera->GetVertical());
	materialShader.SetVec3f("camera.right", camera->GetRight());
	materialShader.SetVec3f("camera.up", camera->GetUp());
	materialShader.SetVec3f("camera.front", camera->GetFront());
	materialShader.SetFloat("camera.lenR", camera->GetLenR());
	materialShader.SetFloat("camera.t0", camera->GetT1());
	materialShader.SetFloat("camera.t1", camera->GetT0());

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
			materialShader.SetFloat("rdSeed[0]", Math::Rand_F());
			materialShader.SetFloat("rdSeed[1]", Math::Rand_F());
			materialShader.SetFloat("rdSeed[2]", Math::Rand_F());
			materialShader.SetFloat("rdSeed[3]", Math::Rand_F());
			VAO_Screen.Draw(materialShader);

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
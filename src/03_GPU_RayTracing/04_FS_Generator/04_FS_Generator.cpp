#include "Defines.h"
#include "CreateScene.h"

#include <RayTracing/FS_Generator.h>
#include <RayTracing/ConstTexture.h>
#include <RayTracing/Dielectric.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/Group.h>
#include <RayTracing/TexWindow.h>
#include <RayTracing/TRayCamera.h>

#include <OpenGL/Texture.h>
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
	const int width = texWindow.GetWidth();
	const int height = texWindow.GetHeight();
	const float ratioWH = width / (float)height;

	//------------ Ä£ÐÍ . Screen
	VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });
	
	//------------ Scene
	Scene::CPtr scene = CreateScene0(ratioWH);
	
	//------------ Fragment Shader Generator
	FS_Generator fsGenerator(scene->obj);
	vector<float> sceneData(fsGenerator.GetSceneData());
	printf("size: %d\n", sceneData.size());
	for (size_t i = 0; i < sceneData.size(); i++)
		printf("%f, ", sceneData[i]);
	printf("\n");
	vector<float> matData(fsGenerator.GetMatData());
	printf("size: %d\n", matData.size());
	for (size_t i = 0; i < matData.size(); i++)
		printf("%f, ", matData[i]);
	printf("\n");
	vector<float> texData(fsGenerator.GetTexData());
	printf("size: %d\n", texData.size());
	for (size_t i = 0; i < texData.size(); i++)
		printf("%f, ", texData[i]);
	printf("\n");
	size_t i;
	i = 1;
	while (i < sceneData.size())
		i <<= 1;
	sceneData.resize(i);
	printf("size: %d\n", sceneData.size());
	i = 1;
	while (i < matData.size())
		i <<= 1;
	matData.resize(i);
	printf("size: %d\n", matData.size());
	i = 1;
	while (i < texData.size())
		i <<= 1;
	texData.resize(i);
	printf("size: %d\n", texData.size());
	for (size_t i = 0; i < texData.size(); i++)
		printf("%f, ", texData[i]);
	printf("\n");
	CppUtility::OpenGL::Texture texDataTex(texData.size(), 1, texData.data(), GL_FLOAT, GL_RED, GL_R32F);
	CppUtility::OpenGL::Texture sceneDataTex(sceneData.size(), 1, sceneData.data(), GL_FLOAT, GL_RED, GL_R32F);
	CppUtility::OpenGL::Texture matDataTex(matData.size(), 1, matData.data(), GL_FLOAT, GL_RED, GL_R32F);
	sceneDataTex.Use(6);
	matDataTex.Use(5);
	texDataTex.Use(4);
	//------------ RayTracing Basic Shader
	string RTX_vs = rootPath + str_RTX_vs;
	string RTX_fs = rootPath + str_RTX_fs;
	//string RTX_fs_src = fsGenerator.BuildFS();
	//File RTX_fs_file(RTX_fs, File::WRITE);
	//RTX_fs_file.Printf("%s", RTX_fs_src.c_str());
	//RTX_fs_file.Close();
	Shader RTX_Shader(RTX_vs, RTX_fs);
	if (!RTX_Shader.IsValid()) {
		printf("ERROR: RTX_Shader load fail.\n");
		return 1;
	}
	//const vec3 pos(0, 0, 0);
	//const vec3 viewPoint(0, 0, -1);
	//const float fov = 90.0f;
	//auto camera = ToCPtr(new TRayCamera(pos, viewPoint, ratioWH, 0, 0, 90.0f));
	auto camera = scene->camera;
	TRayCamera::CPtr tCamera = std::dynamic_pointer_cast<const TRayCamera>(camera);
	const float RayNumMax = 10000.0f;

	RTX_Shader.SetInt("origin_curRayNum", 0);
	RTX_Shader.SetInt("dir_tMax", 1);
	RTX_Shader.SetInt("color_time", 2);
	RTX_Shader.SetInt("rayTracingRst", 3);
	RTX_Shader.SetInt("SceneData", 6);
	RTX_Shader.SetInt("MatData", 5);
	RTX_Shader.SetInt("TexData", 4);
	RTX_Shader.SetFloat("RayNumMax", RayNumMax);
	RTX_Shader.SetVec3f("camera.pos", camera->GetPos());
	RTX_Shader.SetVec3f("camera.BL_Corner", camera->GetBL_Corner());
	RTX_Shader.SetVec3f("camera.horizontal", camera->GetHorizontal());
	RTX_Shader.SetVec3f("camera.vertical", camera->GetVertical());
	RTX_Shader.SetVec3f("camera.right", camera->GetRight());
	RTX_Shader.SetVec3f("camera.up", camera->GetUp());
	RTX_Shader.SetVec3f("camera.front", camera->GetFront());
	RTX_Shader.SetFloat("camera.lenR", camera->GetLenR());
	RTX_Shader.SetFloat("camera.t0", tCamera ? tCamera->GetT0():0);
	RTX_Shader.SetFloat("camera.t1", tCamera ? tCamera->GetT1():0);

	
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
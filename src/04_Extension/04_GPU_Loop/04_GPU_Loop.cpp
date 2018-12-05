#include "Defines.h"
#include "CreateScene.h"

#include <RayTracing/Ray.h>
#include <RayTracing/RayCamera.h>
#include <RayTracing/GenData.h>
#include <RayTracing/ConstTexture.h>
#include <RayTracing/Dielectric.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/Group.h>
#include <RayTracing/TexWindow.h>
#include <RayTracing/TRayCamera.h>

#include <OpenGL/Shader.h>
#include <OpenGL/Texture.h>
#include <OpenGL/VAO.h>
#include <OpenGL/FBO.h>
#include <OpenGL/CommonDefine.h>

#include <Utility/OpQueue.h>
#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/Timer.h>
#include <Utility/Math.h>
#include <Utility/File.h>

#include <glm/glm.hpp>

using namespace RayTracing;
using namespace CppUtility;
using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
using namespace glm;
using namespace std;

size_t Fit2Square(vector<float> & data);

int main(int argc, char ** argv) {
	//------------ Init
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
	Scene::CPtr scene = CreateScene8(ratioWH);


	//------------ Fragment Shader Generator
	GenData genData(scene->obj);

	auto sceneData = genData.GetSceneData();
	auto sceneDataSize = Fit2Square(sceneData);
	OpenGL::Texture sceneDataTex(sceneDataSize, sceneDataSize, sceneData.data(), GL_FLOAT, GL_RED, GL_R32F);

	auto matData = genData.GetMatData();
	auto matDataSize = Fit2Square(matData);
	OpenGL::Texture matDataTex(matDataSize, matDataSize, matData.data(), GL_FLOAT, GL_RED, GL_R32F);

	auto texData = genData.GetTexData();
	auto texDataSize = Fit2Square(texData);
	OpenGL::Texture texDataTex(texDataSize, texDataSize, texData.data(), GL_FLOAT, GL_RED, GL_R32F);

	auto packData = genData.GetPackData();
	auto packDataSize = Fit2Square(packData);
	OpenGL::Texture packDataTex((packDataSize+1) / 2, (packDataSize+1) / 2, packData.data(), GL_FLOAT, GL_RGBA, GL_RGBA32F);

	OpenGL::Texture skyboxTex(genData.GetSkyboxImgs());

	auto img2idx = genData.GetImgIdxMap();
	const size_t texArrSize = 16;
	OpenGL::Texture texArr[texArrSize];
	for (auto const & pair : img2idx) {
		auto img = pair.first;
		texArr[pair.second] = OpenGL::Texture(OpenGL::Texture::ENUM_TYPE_2D_DYNAMIC);
		texArr[pair.second].SetImg(*img);
		//printf("Set TexArr[%d]\n", pair.second);
	}

	/*
	printf("size: %d\n", genData.GetSceneData().size());
	for (size_t i = 0; i < genData.GetSceneData().size(); i++)
		printf("%f, ", genData.GetSceneData()[i]);
	printf("\n");
	
	*/
	printf("size: %d\n", genData.GetMatData().size());
	for (size_t i = 0; i < genData.GetMatData().size(); i++)
		printf("%f, ", genData.GetMatData()[i]);
	printf("\n");
	printf("size: %d\n", genData.GetTexData().size());
	for (size_t i = 0; i < genData.GetTexData().size(); i++)
		printf("%f, ", genData.GetTexData()[i]);
	printf("\n");
	
	printf("size: %d\n", genData.GetPackData().size());
	/*
	for (size_t i = 0; i < genData.GetPackData().size(); i += 4) {
		printf("%f, ", genData.GetPackData()[i]);
		printf("%f, ", genData.GetPackData()[i + 1]);
		printf("%f, ", genData.GetPackData()[i + 2]);
		printf("%f,\n", genData.GetPackData()[i + 3]);
	}
	printf("\n");
	*/
	//return 0;


	//------------ RayTracing Basic Shader
	string RTX_vs = rootPath + str_RTX_vs;
	string RTX_fs = rootPath + str_RTX_fs;
	Shader RTX_Shader(RTX_vs, RTX_fs);
	if (!RTX_Shader.IsValid()) {
		printf("ERROR: RTX_Shader load fail.\n");
		return 1;
	}
	auto camera = scene->camera;
	TRayCamera::CPtr tCamera = std::dynamic_pointer_cast<const TRayCamera>(camera);

	RTX_Shader.SetInt("SumColor", 0);

	RTX_Shader.SetInt("SceneData", 1);
	RTX_Shader.SetInt("MatData", 2);
	RTX_Shader.SetInt("TexData", 3);
	RTX_Shader.SetInt("PackData", 4);
	RTX_Shader.SetInt("skybox", 5);
	for (size_t i = 0; i < texArrSize; i++)
		RTX_Shader.SetInt(string("TexArr[") + to_string(i) + "]", 6 + i);

	RTX_Shader.SetVec3f("camera.pos", camera->GetPos());
	RTX_Shader.SetVec3f("camera.BL_Corner", camera->GetBL_Corner());
	RTX_Shader.SetVec3f("camera.horizontal", camera->GetHorizontal());
	RTX_Shader.SetVec3f("camera.vertical", camera->GetVertical());
	RTX_Shader.SetVec3f("camera.right", camera->GetRight());
	RTX_Shader.SetVec3f("camera.up", camera->GetUp());
	RTX_Shader.SetVec3f("camera.front", camera->GetFront());
	RTX_Shader.SetFloat("camera.lenR", camera->GetLenR());
	RTX_Shader.SetFloat("camera.t0", tCamera ? tCamera->GetT0() : 0);
	RTX_Shader.SetFloat("camera.t1", tCamera ? tCamera->GetT1() : 0);
	
	//------------ Average Shader
	string average_vs = rootPath + str_Average_vs;
	string average_fs = rootPath + str_Average_fs;
	Shader average_Shader(average_vs, average_fs);
	if (!average_Shader.IsValid()) {
		printf("ERROR: RTX_Shader load fail.\n");
		return 1;
	}
	average_Shader.SetInt("texture0", 0);

	//------------ RayTracing FBO
	bool curReadFBO = false;
	bool curWriteFBO = !curReadFBO;
	FBO FBO_RayTracing[2] = {
		FBO(width, height, FBO::ENUM_TYPE_RTX),
		FBO(width, height, FBO::ENUM_TYPE_RTX),
	};
	FBO FBO_Average(width, height, FBO::ENUM_TYPE_COLOR);


	//------------ ²Ù×÷
	Timer timer;
	timer.Start();
	const size_t maxLoopNum = 5000;
	size_t allLoopNum = 0;
	LambdaOp::Ptr RTX_Op = ToPtr(new LambdaOp([&]() {
		size_t loopNum = static_cast<size_t>(glm::max(texWindow.GetScale(), 2.0));
		for (size_t i = 0; i < loopNum; i++) {
			FBO_RayTracing[curWriteFBO].Use();
			FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
			sceneDataTex.Use(1);
			matDataTex.Use(2);
			texDataTex.Use(3);
			packDataTex.Use(4);
			skyboxTex.Use(5);
			for (size_t i = 0; i < texArrSize; i++)
				texArr[i].Use(6 + i);
			RTX_Shader.SetFloat("rdSeed[0]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[1]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[2]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[3]", Math::Rand_F());
			VAO_Screen.Draw(RTX_Shader);

			curReadFBO = curWriteFBO;
			curWriteFBO = !curReadFBO;
		}

		allLoopNum += loopNum;
		double curStep = allLoopNum / (double)maxLoopNum * 100.0;
		double wholeTime = timer.GetWholeTime();
		double speed = allLoopNum / wholeTime;
		double needTime = (maxLoopNum - allLoopNum) / speed;
		printf("\rINFO: %.2f%%, speed %.2f loop / s, used time: %.2f s, need time: %.2f s, whole time: %.2f s     ",
			curStep, speed, wholeTime, needTime, wholeTime + needTime);

		if (allLoopNum >= maxLoopNum)
			RTX_Op->SetIsHold(false);
	}));

	auto averageOp = ToPtr(new LambdaOp([&]() {
		FBO_Average.Use();
		FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
		average_Shader.SetInt("num", allLoopNum);
		VAO_Screen.Draw(average_Shader);

		texWindow.SetTex(FBO_Average.GetColorTexture(0));
	}));

	auto texUpdateOp = ToPtr(new OpQueue);
	(*texUpdateOp) << RTX_Op << averageOp;

	bool success = texWindow.Run(texUpdateOp);
	return success ? 0 : 1;
}

size_t Fit2Square(vector<float> & data) {
	size_t n = static_cast<size_t>(sqrt(data.size()));
	if (n*n < data.size())
		n++;
	data.resize(n*n);
	return n;
}
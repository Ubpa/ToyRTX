#include "Defines.h"
#include "CreateScene.h"

#include <Utility/RTX/GenData.h>
#include <Utility/RTX/ConstTexture.h>
#include <Utility/RTX/Dielectric.h>
#include <Utility/RTX/Metal.h>
#include <Utility/RTX/Lambertian.h>
#include <Utility/RTX/Sphere.h>
#include <Utility/RTX/Group.h>
#include <Utility/RTX/TexWindow.h>
#include <Utility/RTX/TRayCamera.h>

#include <Utility/OGL/Shader.h>
#include <Utility/OGL/Camera.h>
#include <Utility/OGL/Texture.h>
#include <Utility/OGL/VAO.h>
#include <Utility/OGL/FBO.h>
#include <Utility/OGL/CommonDefine.h>

#include <GLFW/Glfw.h>

#include <Utility/Basic/OpQueue.h>
#include <Utility/Basic/EventManager.h>
#include <Utility/Basic/GStorage.h>
#include <Utility/Basic/LambdaOp.h>
#include <Utility/Basic/Timer.h>
#include <Utility/Basic/Math.h>
#include <Utility/Basic/File.h>

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
	Timer timer;
	TexWindow texWindow(str_WindowTitle);
	if (!texWindow.IsValid()) {
		printf("ERROR: Texture Window Create Fail.\n");
		return 1;
	}
	string rootPath = texWindow.GetRootPath();
	const int width = texWindow.GetWidth();
	const int height = texWindow.GetHeight();
	const float ratioWH = width / (float)height;
	Glfw::GetInstance()->LockCursor();

	//------------ Scene
	Scene::CPtr scene = CreateScene0(ratioWH);

	//------------ Move
	vec3 front = normalize(scene->camera->GetFront());
	float pitch = asin(front.y) / Math::PI * 180;
	float yaw = acos(front.x / cos(pitch / 180 * Math::PI)) / Math::PI * 180;
	Camera roamCamera(scene->camera->GetPos(), yaw, pitch, ratioWH, 1, 100);
	size_t moveKey[] = { GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_A, GLFW_KEY_E, GLFW_KEY_Q };
	size_t arrowKey[] = { GLFW_KEY_DOWN, GLFW_KEY_UP, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, GLFW_KEY_PAGE_DOWN, GLFW_KEY_PAGE_UP };
	for (size_t i = 0; i < 6; i++) {
		LambdaOp::Ptr op = ToPtr(new LambdaOp([=, &roamCamera, &timer]() {
			roamCamera.ProcessKeyboard(Camera::ENUM_Movement(Camera::MOVE_FORWARD + i), timer.GetLog(1));
		}));

		EventManager::GetInstance()->Register(EventManager::KEYBOARD | moveKey[i],
			op);
		EventManager::GetInstance()->Register(EventManager::KEYBOARD | arrowKey[i],
			op);
	}
	EventManager::GetInstance()->Register(EventManager::MOUSE_MOUVE, [&]() {
		auto xOffset = **GStorage<float *>::GetInstance()->GetPtr("mousePos_XOffset");
		auto yOffset = **GStorage<float *>::GetInstance()->GetPtr("mousePos_YOffset");
		roamCamera.ProcessMouseMovement(xOffset, -yOffset); 
	});


	//------------ Ä£ÐÍ . Screen
	VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });


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
	
	printf("size: %d\n", genData.GetMatData().size());
	for (size_t i = 0; i < genData.GetMatData().size(); i++)
		printf("%f, ", genData.GetMatData()[i]);
	printf("\n");

	printf("size: %d\n", genData.GetTexData().size());
	for (size_t i = 0; i < genData.GetTexData().size(); i++)
		printf("%f, ", genData.GetTexData()[i]);
	printf("\n");
	
	printf("size: %d\n", genData.GetPackData().size());
	
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
	RTX_Shader.SetInt("RayData0", 1);
	RTX_Shader.SetInt("RayData1", 2);
	RTX_Shader.SetInt("RayData2", 3);

	RTX_Shader.SetInt("SceneData",4);
	RTX_Shader.SetInt("MatData", 5);
	RTX_Shader.SetInt("TexData", 6);
	RTX_Shader.SetInt("PackData", 7);

	RTX_Shader.SetInt("skybox", 8);

	for (size_t i = 0; i < texArrSize; i++)
		RTX_Shader.SetInt(string("TexArr[") + to_string(i) + "]", 9 + i);

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
	FBO FBO_RayTracing[2] = { FBO(width, height, FBO::ENUM_TYPE_RAYTRACING),FBO(width, height, FBO::ENUM_TYPE_RAYTRACING) };
	FBO FBO_Average(width, height, FBO::ENUM_TYPE_COLOR);


	//------------ ²Ù×÷

	size_t allLoopNum = 0;
	size_t frameLoop = 0;

	LambdaOp::Ptr cameraUpdateOp = ToPtr(new LambdaOp([&]() {
		timer.Log();
		static vec3 lastPos(0);
		static vec3 lastViewPoint(0);
		static mat4 viewProjectMat = roamCamera.GetProjectionMatrix() * roamCamera.GetViewMatrix();


		if (lastPos == roamCamera.GetPos() && lastViewPoint == roamCamera.GetPos() - roamCamera.GetFront() )
			return;

		lastPos = roamCamera.GetPos();
		lastViewPoint = roamCamera.GetPos() - roamCamera.GetFront();
		TRayCamera::CPtr camera = ToCPtr(new TRayCamera(lastPos, lastViewPoint, ratioWH));
		RTX_Shader.SetVec3f("camera.pos", camera->GetPos());
		RTX_Shader.SetVec3f("camera.BL_Corner", camera->GetBL_Corner());
		RTX_Shader.SetVec3f("camera.horizontal", camera->GetHorizontal());
		RTX_Shader.SetVec3f("camera.vertical", camera->GetVertical());
		RTX_Shader.SetVec3f("camera.right", camera->GetRight());
		RTX_Shader.SetVec3f("camera.up", camera->GetUp());
		RTX_Shader.SetVec3f("camera.front", camera->GetFront());
		RTX_Shader.SetFloat("camera.lenR", camera->GetLenR());
		RTX_Shader.SetFloat("camera.t0", camera->GetT0());
		RTX_Shader.SetFloat("camera.t1", camera->GetT1());
		RTX_Shader.SetMat4f("oldViewProjectMat", viewProjectMat);

		viewProjectMat = roamCamera.GetProjectionMatrix() * roamCamera.GetViewMatrix();

		RTX_Shader.SetFloat("clear", 1.0);
		frameLoop = 0;
	}));

	LambdaOp::Ptr RTX_Op = ToPtr(new LambdaOp([&]() {
		size_t loopNum = 6;// static_cast<size_t>(glm::max(texWindow.GetScale(), 2.0));
		for (size_t i = 0; i < loopNum; i++) {
			FBO_RayTracing[!curReadFBO].Use();
			FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
			FBO_RayTracing[curReadFBO].GetColorTexture(1).Use(1);
			FBO_RayTracing[curReadFBO].GetColorTexture(2).Use(2);
			FBO_RayTracing[curReadFBO].GetColorTexture(3).Use(3);
			sceneDataTex.Use(4);
			matDataTex.Use(5);
			texDataTex.Use(6);
			packDataTex.Use(7);
			skyboxTex.Use(8);
			for (size_t i = 0; i < texArrSize; i++)
				texArr[i].Use(9 + i);
			RTX_Shader.SetFloat("rdSeed[0]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[1]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[2]", Math::Rand_F());
			RTX_Shader.SetFloat("rdSeed[3]", Math::Rand_F());
			VAO_Screen.Draw(RTX_Shader);
			RTX_Shader.SetFloat("clear", 0.0);
			curReadFBO = !curReadFBO;
		}
		allLoopNum += loopNum;
		double wholeTime = timer.GetWholeTime();
		double speed = allLoopNum / wholeTime;
		printf("\rINFO: %.2f spps, speed %.2f loop / s, used time: %.2f s     ",
			allLoopNum/wholeTime , speed, wholeTime);
	}));

	auto averageOp = ToPtr(new LambdaOp([&]() {
		FBO_Average.Use();
		FBO_RayTracing[curReadFBO].GetColorTexture(0).Use(0);
		VAO_Screen.Draw(average_Shader);

		texWindow.SetTex(FBO_Average.GetColorTexture(0));
	}));

	auto texUpdateOp = ToPtr(new OpQueue);
	(*texUpdateOp) << cameraUpdateOp << RTX_Op << averageOp;

	timer.Start();
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
#include <GLFW/Glfw.h>

#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/Config.h>
#include <Utility/OpNode.h>
#include <Utility/Timer.h>

#include <OpenGL/Texture.h>
#include <OpenGL/FBO.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <iostream>

#include "Defines.h"
#include "RegisterInput.h"

using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
using namespace std;

int main(int argc, char ** argv) {
	Config * config = DoConfig(); 
	string rootPath = *config->GetStrPtr(str_RootPath);
	GStorage<Config *>::GetInstance()->Register(str_MainConfig, config);
	GStorage<string>::GetInstance()->Register(str_RootPath, rootPath);
	
	 
	//------------ 窗口
	Glfw::GetInstance()->Init(val_windowWidth, val_windowHeight, str_WindowTitle);
	Glfw::GetInstance()->LockCursor();


	//------------ 模型 . Screen
	VAO VAO_FlipScreen(&(data_Flip_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });
	VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });

	//------------ Screen Shader
	string screen_vs = rootPath + str_Screen_vs;
	string screen_fs = rootPath + str_Screen_fs;
	Shader screenShader(screen_vs, screen_fs);
	if (!screenShader.IsValid()) {
		printf("ERROR: screenShader load fail\n");
		return 1;
	}
	screenShader.SetInt("texture0", 0);

	//------------ ppBlur Shader
	string ppBlur_vs = rootPath + str_ppBlur_vs;
	string ppBlur_fs = rootPath + str_ppBlur_fs;
	Shader ppBlurShader(ppBlur_vs, ppBlur_fs);
	if (!ppBlurShader.IsValid()) {
		printf("ERROR: ppBlurShader load fail\n");
		return 1;
	}
	ppBlurShader.SetInt("image", 0);


	//------------ 纹理
	Image img(val_ImgWidth, val_ImgHeight, val_ImgChannel);
	Texture tex(Texture::ENUM_TYPE_2D_DYNAMIC);
	tex.SetImg(img);


	//------------ 缓冲
	FBO ppBlurFBOs[2] = {
		FBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR_FLOAT),
		FBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR_FLOAT)
	};


	//------------ 操作

	// 注册输入

	auto registerInputOp = new RegisterInput(false);
	

	// 更新

	float deltaTime = 0.0f; // 当前帧与上一帧的时间差
	size_t curFrame = 0;
	const size_t fps = 60;
	const float tps = 1.0f / fps;
	Timer mainTimer;
	mainTimer.Start();
	GStorage<float *>::GetInstance()->Register(str_DeltaTime.c_str(), &deltaTime);
	auto timeUpdate = new LambdaOp([&]() {
		deltaTime = mainTimer.Log();
		curFrame = mainTimer.GetWholeTime() / tps;
	});

	auto imgUpdate = Operation::ToPtr(new LambdaOp([&]() {
		for (size_t i = 0; i < img.GetWidth(); i++) {
			for (size_t j = 0; j < img.GetHeight(); j++) {
				float r = i / (float)img.GetWidth();
				float g = j / (float)img.GetHeight();
				float b = 0.2;
				img.SetPixel(i, j, Image::Pixel<float>(r, g, b));
			}
		}
	}, false));

	auto texUpdate = Operation::ToPtr(new LambdaOp([&]() {
		static size_t lastFrame = 0;
		if (curFrame <= lastFrame)
			return;

		lastFrame = curFrame;
		tex.SetImg(img);
	}));

	auto updateOpQueue = new OpQueue;
	(*updateOpQueue) << timeUpdate << imgUpdate << texUpdate;
	
	// 渲染
	auto ppBlurOp = Operation::ToPtr(new LambdaOp([&]() {
		bool horizontal = true;
		bool first_iteration = true;
		for (size_t i = 0; i < 4; i++) {
			ppBlurFBOs[horizontal].Use();
			ppBlurShader.SetBool("horizontal", horizontal);
			if (first_iteration)
				tex.Use();
			else
				ppBlurFBOs[!horizontal].GetColorTexture().Use();
			if (first_iteration)
				VAO_FlipScreen.Draw();
			else
				VAO_Screen.Draw();
			horizontal = !horizontal;
			first_iteration = false;
		}
		FBO::UseDefault();
	}));

	auto screenOp = Operation::ToPtr(new LambdaOp([&]() {
		ppBlurFBOs[0].GetColorTexture().Use();
		screenShader.Use();
		VAO_Screen.Draw();
	}));

	auto defaultBufferOp = new OpNode([]() {//init
		FBO::UseDefault();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}, []() {//end
		glfwSwapBuffers(Glfw::GetInstance()->GetWindow());
		glfwPollEvents();
	});

	(*defaultBufferOp) << ppBlurOp << screenOp;

	auto renderQueue = new OpQueue;
	(*renderQueue) << defaultBufferOp;
	
	// 帧操作

	auto opQueue = new OpQueue;
	(*opQueue) << registerInputOp << updateOpQueue << renderQueue;
	
	//------------
	Glfw::GetInstance()->Run(opQueue);
	
	//------------
	ppBlurFBOs->Use();
	Image blurImg(val_ImgWidth, val_ImgHeight, val_ImgChannel);
	glReadPixels(0, 0, val_ImgWidth, val_ImgHeight, GL_RGB, GL_UNSIGNED_BYTE, blurImg.GetData());

	Glfw::GetInstance()->Terminate();

	img.SaveAsPNG(rootPath + "/data/out/" + str_WindowTitle + ".png", false);
	blurImg.SaveAsPNG(rootPath + "/data/out/" + str_WindowTitle + "blur.png", true);
	return 0;
}


#include <RayTracing/ImgWindow.h>

#include <ROOT_PATH.h>

#include <GLFW/Glfw.h>

#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/Config.h>
#include <Utility/OpNode.h>
#include <Utility/Timer.h>

#include <OpenGL/CommonDefine.h>
#include <OpenGL/Texture.h>
#include <OpenGL/FBO.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

using namespace RayTracing;
using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
using namespace std;

CppUtility::Other::Config * DoConfig();

ImgWindow::ImgWindow(const string & title, size_t fps, ENUM_OPTION option)
	: title(title), option(option), fps(fps), scale(1.0f){
	Config * config = DoConfig();
	if (config == NULL) {
		isValid = false;
		return;
	}

	auto pRootPath = config->GetStrPtr(str_RootPath);
	if (pRootPath == NULL) {
		printf("ERROR: Config [string RootPath] is not exist.\n");
		isValid = false;
		return;
	}
	rootPath = *pRootPath;

	auto pDefaultWidth = config->GetIntPtr("val_Default_ImgWidth");
	auto pDefaultHeight = config->GetIntPtr("val_Default_ImgHeight");
	auto pWidth = config->GetIntPtr("val_" + title + "_ImgWidth");
	auto pHeight = config->GetIntPtr("val_" + title + "_ImgHeight");

	if (pWidth == NULL && pDefaultWidth == NULL) {
		printf("ERROR: Config [int val_%s_ImgWidth] and [val_Default_ImgWidth] are not exist.\n", title.c_str());
		isValid = false;
		return;
	}
	int width = pWidth ? *pWidth : *pDefaultWidth;
	
	if (pHeight == NULL && pDefaultHeight == NULL) {
		printf("ERROR: Config [int val_%s_ImgHeight] and [val_Default_ImgHeight] are not exist.\n", title.c_str());
		isValid = false;
		return;
	}
	int height = pHeight ? *pHeight : *pDefaultHeight;

	img.GenBuffer(width, height, 3);
	if (!img.IsValid()) {
		isValid = false;
		return;
	}

	isValid = true;
}

bool ImgWindow::Run(const Ptr<Operation> & imgUpdateOp) {
	Glfw::GetInstance()->Init(val_windowWidth, val_windowHeight, title);

	//------------ VAO
	VAO VAO_FlipScreen(&(data_Flip_ScreenVertices[0]), sizeof(data_Flip_ScreenVertices), { 2,2 });
	VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });


	//------------ Screen Shader
	string screen_vs = rootPath + str_Screen_vs;
	string screen_fs = rootPath + str_Screen_fs;
	Shader screenShader(screen_vs, screen_fs);
	if (!screenShader.IsValid()) {
		printf("ERROR: screenShader load fail\n");
		return false;
	}
	screenShader.SetInt("texture0", 0);


	//------------ ppBlur Shader
	string ppBlur_vs = rootPath + str_ppBlur_vs;
	string ppBlur_fs = rootPath + str_ppBlur_fs;
	Shader ppBlurShader(ppBlur_vs, ppBlur_fs);
	if (!ppBlurShader.IsValid()) {
		printf("ERROR: ppBlurShader load fail\n");
		return false;
	}
	ppBlurShader.SetInt("image", 0);


	//------------ 图像参数获取
	size_t val_ImgWidth = img.GetWidth();
	size_t val_ImgHeight = img.GetHeight();
	size_t val_ImgChannel = img.GetChannel();


	//------------ 纹理
	Texture tex(Texture::ENUM_TYPE_2D_DYNAMIC);
	tex.SetImg(img);


	//------------ flip 缓冲
	FBO flipFBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_BASIC);


	//------------ ppBlur 缓冲
	FBO ppBlurFBOs[2] = {
		FBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR),
		FBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR)
	};


	//------------ 操作

	// 注册输入

	EventManager::GetInstance()->Register(EventManager::KEYBOARD_PRESS | GLFW_KEY_ESCAPE,
		[]() { Glfw::GetInstance()->CloseWindow(); });


	// 更新

	double deltaTime = 0.0f;
	size_t curFrame = 0;
	const double tps = 1.0f / fps;
	Timer mainTimer;
	mainTimer.Start();
	auto timeUpdate = new LambdaOp([&]() {
		deltaTime = mainTimer.Log();
		scale *= tps / deltaTime;
		curFrame = mainTimer.GetWholeTime() / tps;
	});

	const Texture * curTex = &tex;
	auto texUpdate = Operation::ToPtr(new LambdaOp([&]() {
		static size_t lastFrame = 0;
		if (curFrame <= lastFrame)
			return;

		lastFrame = curFrame;
		tex.SetImg(img);
		curTex = &tex;
	}));

	auto updateOpQueue = new OpQueue;
	(*updateOpQueue) << timeUpdate << imgUpdateOp << texUpdate;

	// 渲染
	const FBO * curFBO = NULL;

	auto ppBlurOp = Operation::ToPtr(new LambdaOp([&]() {
		bool horizontal = true;
		for (size_t i = 0; i < 4; i++) {
			curFBO = &ppBlurFBOs[horizontal];
			curFBO->Use();
			ppBlurShader.SetBool("horizontal", horizontal);
			curTex->Use();
			VAO_Screen.Draw(ppBlurShader);
			curTex = &curFBO->GetColorTexture();
			horizontal = !horizontal;
		}
	}));

	auto screenOp = Operation::ToPtr(new LambdaOp([&]() {
		FBO::UseDefault();
		curTex->Use();
		if ((option & ENUM_OPTION_POST_PROCESS_FLIP) != 0)
			VAO_FlipScreen.Draw(screenShader);
		else
			VAO_Screen.Draw(screenShader);
	}));

	auto finalOp = Operation::ToPtr(new LambdaOp([&]() {
		glfwSwapBuffers(Glfw::GetInstance()->GetWindow());
		glfwPollEvents();
	}));

	GLint viewport[4];
	auto imgProcessOp = new OpNode([&]() {
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, val_ImgWidth, val_ImgHeight);
	}, [&](){
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	});

	auto renderQueue = new OpQueue;
	if ((option & ENUM_OPTION_POST_PROCESS_BLUR) != 0)
		(*imgProcessOp) << ppBlurOp;
	(*renderQueue) << imgProcessOp << screenOp;

	// 帧操作

	auto opQueue = new OpQueue;
	(*opQueue) << updateOpQueue << renderQueue << finalOp;

	//------------
	Glfw::GetInstance()->Run(opQueue);

	//------------
	if ((option & ENUM_OPTION_SAVE_SRC_IMG) != 0)
		img.SaveAsPNG(rootPath + "/data/out/" + title + ".png", true);

	if ((option & ENUM_OPTION_SAVE_POST_PROCESS_IMG) != 0 && (option & ENUM_OPTION_POST_PROCESS_ALL) != 0) {
		curFBO->Use();
		Image finalImg(val_ImgWidth, val_ImgHeight, val_ImgChannel);
		glReadPixels(0, 0, val_ImgWidth, val_ImgHeight, GL_RGB, GL_UNSIGNED_BYTE, finalImg.GetData());
		finalImg.SaveAsPNG(rootPath + "/data/out/" + title + "_blur.png");
	}

	Glfw::GetInstance()->Terminate();



	scale = 1.0;
	return true;
}

Config * DoConfig() {
	printf("INFO: Try to read config.out\n");
	string rootPath;
	Config * config = new Config;

	rootPath = string(ROOT_PATH);
	printf("INFO: [1] First Try.\n");
	config->Load(rootPath + "/config/config.out");
	if (!config->IsValid()) {
		rootPath = "../../..";
		printf("INFO: [2] Second Try.\n");
		config->Load(rootPath + "/config/config.out");
		if (!config->IsValid()) {
			rootPath = ".";
			printf("INFO: [3] Third Try.\n");
			config->Load(rootPath + "/config.out");
			if (!config->IsValid()) {
				printf(
					"ERROR: Three Try All Fail\n"
					"ERROR: RootPath is not valid.\n"
					"INFO: Please change config/config.out 's value of RootPath and\n"
					"      run exe in correct place( original place or same palce with config.out ).\n");
				return NULL;
			}
		}
	}

	*config->GetStrPtr("RootPath") = rootPath;
	printf("INFO: config.out read success\nINFO: RootPath is %s\n", config->GetStrPtr("RootPath")->c_str());
	GStorage<Config *>::GetInstance()->Register(str_MainCamera, config);
	return config;
}
#include <RayTracing/ImgWindow.h>

#include <OpenGL/CommonDefine.h>
#include <OpenGL/Texture.h>
#include <OpenGL/FBO.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <Utility/GStorage.h>
#include <Utility/LambdaOp.h>
#include <Utility/OpNode.h>
#include <Utility/Timer.h>

#include <GLFW/Glfw.h>

#include <ROOT_PATH.h>

using namespace RayTracing;
using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
using namespace std;

ImgWindow::ImgWindow(const string & title)
	: title(title), scale(1.0f){
	Ptr<Config> config = DoConfig();
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

	auto pBlurNum = config->GetIntPtr("blurNum");
	blurNum = pBlurNum ? *pBlurNum : 2;

	auto pOption = config->GetUnsignedIntPtr("option");
	option = static_cast<ENUM_OPTION>(pOption ? *pOption : ENUM_OPTION_BASIC);

	auto pFPS = config->GetIntPtr("FPS");
	fps = pFPS ? *pFPS : 8;

	isValid = true;
}

bool ImgWindow::Run(const Ptr<Operation> & imgUpdateOp) {
	Glfw::GetInstance()->Init(img.GetWidth(), img.GetHeight(), title);


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


	//------------ interpolation Shader
	string interpolation_vs = rootPath + str_Interpolation_vs;
	string interpolation_fs = rootPath + str_Interpolation_fs;
	Shader interpolationShader(interpolation_vs, interpolation_fs);
	if (!interpolationShader.IsValid()) {
		printf("ERROR: interpolationShader load fail\n");
		return false;
	}
	interpolationShader.SetInt("image", 0);


	//------------ Í¼Ïñ²ÎÊý»ñÈ¡
	size_t val_ImgWidth = img.GetWidth();
	size_t val_ImgHeight = img.GetHeight();
	size_t val_ImgChannel = img.GetChannel();


	//------------ ÎÆÀí
	Texture tex(Texture::ENUM_TYPE_2D_DYNAMIC);
	tex.SetImg(img);


	//------------ Interpolation »º³å
	FBO interpolationFBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR);
	if (!interpolationFBO.IsValid()) {
		printf("ERROR: Gen interpolationFBO fail\n");
		return false;
	}


	//------------ flip »º³å
	FBO flipFBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR);
	if (!flipFBO.IsValid()) {
		printf("ERROR: Gen flipFBO fail\n");
		return false;
	}


	//------------ ppBlur »º³å
	FBO ppBlurFBOs[2] = {
		FBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR),
		FBO(val_ImgWidth, val_ImgHeight, FBO::ENUM_TYPE_COLOR)
	};
	if (!ppBlurFBOs[0].IsValid() || !ppBlurFBOs[1].IsValid()) {
		printf("ERROR: Gen ppBlurFBOs fail\n");
		return false;
	}


	//------------ ²Ù×÷

	// ×¢²áÊäÈë

	EventManager::GetInstance()->Register(EventManager::KEYBOARD_PRESS | GLFW_KEY_ESCAPE,
		[]() { Glfw::GetInstance()->CloseWindow(); });


	// ¸üÐÂ

	size_t curFrame = 0;
	Timer mainTimer;
	mainTimer.Start();
	auto scaleUpdate = ToPtr(new LambdaOp([&]() {
		double tps = 1.0f / fps;
		double imgUpdateTime = mainTimer.GetLog(1);
		double otherTime = mainTimer.GetLog(2);
		if(tps < otherTime)
			tps = otherTime * 4;
		scale *= imgUpdateTime < 10e-6 ? tps/10e-6 * scale : (tps-otherTime) / imgUpdateTime;
		curFrame = static_cast<size_t>(mainTimer.GetWholeTime() / tps);
	}));

	auto timeLogOp = ToPtr(new LambdaOp([&]() {
		mainTimer.Log();
	}));

	const Texture * curTex = &tex;
	Ptr<Operation> texUpdate = ToPtr(new LambdaOp([&]() {
		static size_t lastFrame = 0;
		if (curFrame <= lastFrame)
			return;

		lastFrame = curFrame;
		tex.SetImg(img);
		curTex = &tex;
		if (!imgUpdateOp->IsHold()) {
			printf("Now the texture is no need to update.\n");
			texUpdate->SetIsHold(false);
		}
	}));

	auto updateOpQueue = ToPtr(new OpQueue);
	(*updateOpQueue) << timeLogOp << imgUpdateOp << timeLogOp << scaleUpdate << texUpdate;

	// äÖÈ¾
	const FBO * curFBO = NULL;

	auto interpolationOp = ToPtr(new LambdaOp([&]() {
		curTex->Use();
		interpolationFBO.Use();
		VAO_Screen.Draw(interpolationShader);
		curTex = &interpolationFBO.GetColorTexture();
		curFBO = &interpolationFBO;
	}));

	auto flipOp = ToPtr(new LambdaOp([&]() {
		curTex->Use();
		flipFBO.Use();
		VAO_FlipScreen.Draw(screenShader);
		curTex = &flipFBO.GetColorTexture();
		curFBO = &flipFBO;
	}));

	auto ppBlurOp = ToPtr(new LambdaOp([&]() {
		bool horizontal = true;
		for (size_t i = 0; i < 2 * blurNum; i++) {
			curFBO = &ppBlurFBOs[horizontal];
			curFBO->Use();
			ppBlurShader.SetBool("horizontal", horizontal);
			curTex->Use();
			VAO_Screen.Draw(ppBlurShader);
			curTex = &curFBO->GetColorTexture();
			horizontal = !horizontal;
		}
	}));

	auto screenOp = ToPtr(new LambdaOp([&]() {
		FBO::UseDefault();
		curTex->Use();
		VAO_Screen.Draw(screenShader);
		curTex = &tex;
	}));

	auto finalOp = ToPtr(new LambdaOp([&]() {
		glfwSwapBuffers(Glfw::GetInstance()->GetWindow());
		glfwPollEvents();
	}));

	GLint viewport[4];
	auto imgProcessOp = ToPtr(new OpNode([&]() {
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, val_ImgWidth, val_ImgHeight);
	}, [&](){
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	}));

	auto renderQueue = new OpQueue;
	if ((option & ENUM_OPTION_POST_PROCESS_INTERPOLATION) != 0)
		(*imgProcessOp) << interpolationOp;
	if ((option & ENUM_OPTION_POST_PROCESS_FLIP) != 0)
		(*imgProcessOp) << flipOp;
	if ((option & ENUM_OPTION_POST_PROCESS_BLUR) != 0)
		(*imgProcessOp) << ppBlurOp;
	(*renderQueue) << imgProcessOp << screenOp;

	// Ö¡²Ù×÷

	auto opQueue = new OpQueue;
	(*opQueue) << updateOpQueue << renderQueue << finalOp;

	//------------ Run
	Glfw::GetInstance()->Run(opQueue);

	//------------ Save Images
	if ((option & ENUM_OPTION_SAVE_SRC_IMG) != 0)
		img.SaveAsPNG(rootPath + "/data/out/" + title + ".png");

	if ((option & ENUM_OPTION_SAVE_POST_PROCESS_IMG) != 0 && (option & ENUM_OPTION_POST_PROCESS_ALL) != 0) {
		bool flip = option & ENUM_OPTION_POST_PROCESS_FLIP ? true : false;
		curFBO->Use();
		Image finalImg(val_ImgWidth, val_ImgHeight, val_ImgChannel);
		glReadPixels(0, 0, val_ImgWidth, val_ImgHeight, GL_RGB, GL_UNSIGNED_BYTE, finalImg.GetData());
		finalImg.SaveAsPNG(rootPath + "/data/out/" + title + "_post.png", flip);
	}

	Glfw::GetInstance()->Terminate();

	scale = 1.0;
	return true;
}

Ptr<Config> ImgWindow::DoConfig() {
	printf("INFO: Try to read config.out\n");
	string rootPath;
	Ptr<Config> config = Ptr<Config>(new Config);

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
	
	auto pRootPath = const_cast<string*>(config->GetStrPtr("RootPath"));
	*pRootPath = rootPath;
	printf("INFO: config.out read success\nINFO: RootPath is %s\n", config->GetStrPtr("RootPath")->c_str());
	GStorage<Ptr<Config>>::GetInstance()->Register(str_MainConfig, config);
	return config;
}
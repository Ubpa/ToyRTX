#include "RegisterInput.h"

#include "Defines.h"

#include <Utility/GStorage.h>

#include <GLFW/Glfw.h>

#include <Utility/LambdaOp.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace CppUtility::OpenGL;
using namespace CppUtility::Other;
using namespace Define;
using namespace std;

void RegisterInput::Run() {
	RegisterMouse();
	RegisterKey();
	PrintInfo();
}

void RegisterInput::RegisterMouse() {
}

void RegisterInput::RegisterKey() {
	// Close Window
	auto closeWindowOp = new LambdaOp();
	EventManager::GetInstance()->Register(EventManager::KEYBOARD_PRESS | GLFW_KEY_ESCAPE,
		[]() { Glfw::GetInstance()->CloseWindow(); });
}

void RegisterInput::PrintInfo() {
	cout << endl
		<< "* 1. Press 'ESC' to close exe" << endl << endl;
}
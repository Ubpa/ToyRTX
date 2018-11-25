#ifndef GLFW_H_
#define GLFW_H_
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Utility/EventManager.h>
#include <string>

namespace CppUtility {
	namespace OpenGL {
		class Glfw {
		public:
			static Glfw * GetInstance();
			//------------
			void Init(size_t width = 800, size_t height = 600, const std::string & title = "Title");
			void Terminate();
			void Run(Other::Operation * op = NULL);
			void Run(Other::Ptr<Other::Operation> op);
			GLFWwindow * GetWindow();
			void CloseWindow();
			void LockCursor();
			int GetKey(int key);
		private:
			Glfw();
			Glfw(const Glfw&);
			Glfw& operator=(const Glfw&) = default;
			static Glfw * instance;
			//------------
			static void CB_FrameBuffSize(GLFWwindow* window, int width, int height);
			void GenWindow(size_t width = 800, size_t height = 600, const std::string & title = "title");
			void LoadGL();
			//------------
			GLFWwindow * window;
		};
	}
}
#endif
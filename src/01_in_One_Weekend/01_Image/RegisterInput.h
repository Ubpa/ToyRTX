#ifndef _REGISTER_INPUT_H_
#define _REGISTER_INPUT_H_

#include <Utility/Operation.h>

class RegisterInput : public CppUtility::Other::Operation {
public:
	RegisterInput(bool isHold = true)
		:Operation(isHold) { }
	void Run();
private:
	void RegisterMouse();
	void RegisterKey();
	void PrintInfo();
};

#endif // !_REGISTER_INPUT_H_

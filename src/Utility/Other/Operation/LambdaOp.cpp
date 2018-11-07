#include <Utility/LambdaOp.h>

using namespace CppUtility::Other;

LambdaOp::LambdaOp(const std::function<void()> & op, bool isHold)
	: op(op), Operation(isHold) { }

LambdaOp::~LambdaOp() { printf("INFO: Delete LambdaOp\n"); };


void LambdaOp::SetOp(const std::function<void()> & op) {
	this->op = op;
}

void LambdaOp::Run() {
	if (op == NULL)
		isHold = false;
	else
		op();
}

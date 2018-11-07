#include <Utility/Operation.h>

using namespace CppUtility::Other;
using namespace std;

Operation::Operation(bool isHold): isHold(isHold) { };

Operation::~Operation() { printf("INFO: Delete Operation\n"); };

void Operation::ProtectDelete(Operation * op) { delete op; }
//------------

bool Operation::IsHold() { return isHold; }

void Operation::SetIsHold(bool isHold) { this->isHold = isHold; }

void Operation::operator()() { Run(); }
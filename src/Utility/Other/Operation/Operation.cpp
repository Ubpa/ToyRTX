#include <Utility/Operation.h>

using namespace CppUtility::Other;
using namespace std;

Operation::Operation(bool isHold): isHold(isHold) { };

//------------

bool Operation::IsHold() { return isHold; }

void Operation::SetIsHold(bool isHold) { this->isHold = isHold; }

void Operation::operator()() { Run(); }
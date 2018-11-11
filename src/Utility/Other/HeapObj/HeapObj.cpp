#include <Utility/HeapObj.h>

using namespace CppUtility::Other;

void HeapObj::ProtectedDelete(HeapObj * op) {
	delete op;
}
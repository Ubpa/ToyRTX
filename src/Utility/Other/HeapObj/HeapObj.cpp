#include <Utility/HeapObj.h>

using namespace CppUtility::Other;

void HeapObj::ProtectedDelete(const HeapObj * op) {
	delete op;
}
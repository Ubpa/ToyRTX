#ifndef _HEAP_OBJ_H_
#define _HEAP_OBJ_H_

#include <Utility/Ptr.h>

#define HEAP_OBJ_SETUP(CLASS) \
public:\
	typedef CppUtility::Other::Ptr<CLASS> Ptr;\
	virtual char * GetClassName(){ return "#CLASS"; }\
protected:\
	virtual ~CLASS() = default;

namespace CppUtility {
	namespace Other {
		template <typename T>
		Ptr<T> ToPtr(T * op) {
			return Ptr<T>(op, T::ProtectedDelete);
		}

		class HeapObj {
			template <typename T>
			friend Ptr<T> ToPtr(T * op);

		protected:
			virtual ~HeapObj() = default;
			static void ProtectedDelete(HeapObj * op);
		};
	}
}

#endif // !_HEAP_OBJ_H_

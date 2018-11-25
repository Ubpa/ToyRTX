#ifndef _HEAP_OBJ_H_
#define _HEAP_OBJ_H_

#include <Utility/Ptr.h>

#define HEAP_OBJ_SETUP(CLASS) \
public:\
	typedef CppUtility::Other::Ptr<CLASS> Ptr;\
	typedef CppUtility::Other::CPtr<CLASS> CPtr;\
	virtual char * GetClassName(){ return "#CLASS"; }\
protected:\
	virtual ~CLASS() = default;

namespace CppUtility {
	namespace Other {
		template <typename T>
		Ptr<T> ToPtr(T * op) {
			return Ptr<T>(op, T::ProtectedDelete);
		}

		template <typename T>
		CPtr<T> ToCPtr(const T * op) {
			return CPtr<T>(op, T::ProtectedDelete);
		}

		class HeapObj {
			template <typename T>
			friend Ptr<T> ToPtr(T * op);
			template <typename T>
			friend CPtr<T> ToCPtr(const T * op);

		protected:
			virtual ~HeapObj() = default;
			static void ProtectedDelete(const HeapObj * op);
		};
	}
}

#endif // !_HEAP_OBJ_H_

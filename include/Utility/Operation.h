#ifndef _OPERATION_H_
#define _OPERATION_H_

#include <string>
#include <Utility/HeapObj.h>

namespace CppUtility {
	namespace Other {
		class Operation : public HeapObj {
			HEAP_OBJ_SETUP(Operation)
		public:
			Operation(bool isHold = true);

			bool IsHold() const;
			void SetIsHold(bool isHold);
			void operator()();
			//------------
			virtual void Run() = 0;
		protected:
			bool isHold;
		private:
			Operation(const Operation&) = delete;
			Operation& operator=(const Operation&) = delete;
		};
	}
}

#endif//! _FILE_H_
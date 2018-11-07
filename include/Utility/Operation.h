#ifndef _OPERATION_H_
#define _OPERATION_H_

#include <string>

namespace CppUtility {
	namespace Other {
		template<typename T>
		using Ptr = std::shared_ptr<T>;

		class Operation {
		public:
			template <typename T>
			static Ptr<T> ToPtr(T * op) {
				return Ptr<T>(op, ProtectDelete);
			}
			//------------
			bool IsHold();
			void SetIsHold(bool isHold);
			void operator()();
			//------------
			virtual void Run() = 0;
		protected:
			Operation(bool isHold = true);
			//------------
			static void ProtectDelete(Operation * op);
			virtual ~Operation();
			//------------
			bool isHold;
		private:
			Operation(const Operation&) = delete;
			Operation& operator=(const Operation&) = delete;
		};
	}
}

#endif//! _FILE_H_
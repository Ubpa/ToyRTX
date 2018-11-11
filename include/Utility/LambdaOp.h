#ifndef _LAMBDA_OP_H_
#define _LAMBDA_OP_H_

#include <Utility/Operation.h>
#include <functional>

namespace CppUtility {
	namespace Other {
		class LambdaOp : public Operation {
			HEAP_OBJ_SETUP(LambdaOp)
		public:
			LambdaOp(const std::function<void()> & op = []() {}, bool isHold = true);
			void SetOp(const std::function<void()> & op);
			//------------
			virtual void Run();
		protected:
			std::function<void()> op;
		private:
			LambdaOp(const LambdaOp&) = delete;
			LambdaOp& operator=(const LambdaOp&) = delete;
		};
	}
}

#endif // !_LAMBDA_OP_H_

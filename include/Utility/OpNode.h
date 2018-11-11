#ifndef _OPNODE_H_
#define _OPNODE_H_

#include <Utility/OpQueue.h>
#include <Utility/LambdaOp.h>

namespace CppUtility {
	namespace Other {
		class OpNode : public OpQueue {
			HEAP_OBJ_SETUP(OpNode)
		public:
			OpNode(const Operation::Ptr & preOp = nullptr, const Operation::Ptr & postOp = nullptr, bool isHold = true);
			OpNode(const std::function<void()> & preOp = nullptr, const std::function<void()> & postOp = nullptr, bool isHold = true);
			OpNode(Operation * preOp = nullptr, Operation * postOp = nullptr, bool isHold = true);
			//------------
			void SetPreOp(const Operation::Ptr & preOp);
			void SetPostOp(const Operation::Ptr & postOp);
			void SetPreOp(Operation * preOp);
			void SetPostOp(Operation * postOp);
			size_t ChildNum();
			virtual void Run();
		protected:
			using OpQueue::Size;
			//------------
			Operation::Ptr preOp;
			Operation::Ptr postOp;
		private:
			OpNode(const OpNode &) = delete;
			OpNode& operator=(const OpNode &) = delete;
		};
	}
}

#endif // !_OPNODE_H_

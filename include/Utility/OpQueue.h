#ifndef _OP_QUEUE_H_
#define _OP_QUEUE_H_

#include <Utility/Operation.h>
#include <list>

namespace CppUtility {
	namespace Other {
		class OpQueue : public Operation {
			HEAP_OBJ_SETUP(OpQueue)
		public:
			OpQueue(bool isHold = true);
			//------------
			OpQueue & operator<<(const Operation::Ptr & operation);
			OpQueue & operator<<(Operation * operation);
			void Push(Operation::Ptr & op);
			void Push(Operation * op);
			size_t Size() const;
			//------------
			virtual void Run();
		protected:
			std::list< Operation::Ptr > opList;
		private:
			OpQueue(const OpQueue &) = delete;
			OpQueue& operator=(const OpQueue &) = delete;
		};
	}
}

#endif // !_OP_QUEUE_H_

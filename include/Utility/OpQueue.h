#ifndef _OP_QUEUE_H_
#define _OP_QUEUE_H_

#include <Utility/Operation.h>
#include <list>

namespace CppUtility {
	namespace Other {
		class OpQueue : public Operation {
		public:
			OpQueue(bool isHold = true);
			//------------
			OpQueue & operator<<(const Ptr<Operation> & operation);
			OpQueue & operator<<(Operation * operation);
			void Push(Ptr<Operation> & op);
			void Push(Operation * op);
			size_t Size() const;
			//------------
			virtual void Run();
		protected:
			virtual ~OpQueue();
			//------------
			std::list< Ptr<Operation> > opList;
		private:
			OpQueue(const OpQueue &) = delete;
			OpQueue& operator=(const OpQueue &) = delete;
		};
	}
}

#endif // !_OP_QUEUE_H_

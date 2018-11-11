#ifndef _INFO_LAMBDA_OP_H_
#define _INFO_LAMBDA_OP_H_

#include <Utility/LambdaOp.h>
#include <functional>

namespace CppUtility {
	namespace Other {
		template<typename T>
		class InfoLambdaOp : public LambdaOp {
			HEAP_OBJ_SETUP(InfoLambdaOp)
		public:
			InfoLambdaOp(const T & info = T(), const std::function<void()> & op = []() {}, bool isHold = true);
			//------------
			void SetInfo(const T & info);
			T & GetInfo();
		protected:
			T info;
		private:
			InfoLambdaOp(const InfoLambdaOp&) = delete;
			InfoLambdaOp& operator=(const InfoLambdaOp&) = delete;
		};

		//------------

		template<typename T>
		InfoLambdaOp<T>::InfoLambdaOp(const T & info, const std::function<void()> & op, bool isHold)
			:info(info), LambdaOp(op, isHold) {
		}

		template<typename T>
		void InfoLambdaOp<T>::SetInfo(const T & info) {
			this->info = info;
		}

		template<typename T>
		T & InfoLambdaOp<T>::GetInfo() {
			return info;
		}
	}
}

#endif // !_INFO_LAMBDA_OP_H_

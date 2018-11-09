#ifndef _PTR_H_
#define _PTR_H_

#include <memory>

namespace CppUtility {
	namespace Other {
		template<typename T>
		using Ptr = std::shared_ptr<T>;
	}
}

#endif // !_PTR_H_

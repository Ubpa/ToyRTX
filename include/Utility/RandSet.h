#ifndef _RAND_SET_
#define _RAND_SET_

#include <random>
#include <vector>

namespace CppUtility {
	namespace Other {
		template <typename T>
		class RandomSet {
		public:
			void Clear() { data.clear(); }
			T RandPick() {
				size_t idx = randMap(randEngine) % data.size();
				T rst = data[idx];
				data[idx] = data.back();
				data.pop_back();
				return rst;
			}
			T Insert
			size_t Size() const { return data.size(); }
		private:
			static std::uniform_int_distribution<unsigned> randMap;
			static std::default_random_engine randEngine;
			std::vector<T> data;
		};
	}
}

#endif // !_RAND_SET_

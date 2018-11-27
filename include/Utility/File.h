#ifndef _FILE_H_
#define _FILE_H_

#include <string>
#include <cstdio>

namespace CppUtility {
	namespace Other {
		class File {
		public:
			enum Mode {
				READ,
				WRITE
			};

			File(const std::string & fileName, Mode mode);

			~File();

			bool Check(Mode mode) const;

			int Printf(const char * format, ...);

			int Scanf(const char * format, ...);

			std::string ReadLine();

			std::string ReadAll();

			bool IsEnd() const;

			bool IsValid() const;

			void Close();
		private:
			std::string ModeToStr(Mode mode);

			FILE * pF;
			Mode mode;
			std::string fileName;
		};
	};
}
#endif//! _FILE_H_
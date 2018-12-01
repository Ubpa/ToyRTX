#ifndef _SKYBOX_H_
#define _SKY_BOX_H_

#include <RayTracing/Texture.h>
#include <Utility/Ptr.h>

#include <vector>

#include <string>

namespace CppUtility {
	namespace Other {
		class Image;
	}
}

namespace RayTracing {
	class Skybox : public Texture {
		TEXTURE_SETUP(Skybox)
	public:
		//right, left, top, bottom, front, back
		Skybox(const std::vector<std::string> & skybox);
		bool IsValid() const;
	private:
		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;
		std::vector<CppUtility::Other::CPtr<CppUtility::Other::Image> > imgs;
	};
}

#endif // !_SKYBOX_H_

#ifndef _IMG_TEXTURE_H_
#define _IMG_TEXTURE_H_

#include <RayTracing/Texture.h>

#include <string>

#include <Utility/Ptr.h>

namespace CppUtility {
	namespace Other {
		class Image;
	}
}

namespace RayTracing {
	class ImgTexture : public Texture{
		TEXTURE_SETUP(ImgTexture)
	public:
		typedef CppUtility::Other::CPtr<CppUtility::Other::Image> ImgCPtr;
		ImgTexture(const std::string & fileName, bool flip = false);

		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;
		bool IsValid();
		const ImgCPtr GetImg() const { return img; }
	private:
		ImgCPtr img;
	};
}

#endif // !_IMG_TEXTURE_H_

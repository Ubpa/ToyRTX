#ifndef _IMG_TEXTURE_H_
#define _IMG_TEXTURE_H_

#include <RayTracing/Texture.h>

#include <Utility/Image.h>

namespace RayTracing {
	class ImgTexture : public Texture{
		TEXTURE_SETUP(ImgTexture)
	public:
		ImgTexture(const std::string & fileName, bool flip = false);

		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;
		bool IsValid();
	private:
		CppUtility::Other::Image img;
	};
}

#endif // !_IMG_TEXTURE_H_

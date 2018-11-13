#ifndef _OP_TEXTURE_H_
#define _OP_TEXTURE_H_

#include <RayTracing/Texture.h>

#include <functional>

namespace RayTracing {
	class OpTexture : public Texture {
		HEAP_OBJ_SETUP(OpTexture)
	public:
		OpTexture(const std::function<glm::rgb(float u, float v, const glm::vec3 & p)> & op);

		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;

		static OpTexture::Ptr ConstantTexture(const glm::rgb & color);
		static OpTexture::Ptr CheckerTexture(const glm::rgb & color0, const glm::rgb & color1, float scale = 10.0f);
		static OpTexture::Ptr NoiseTexture(size_t mode = 0, const glm::rgb & color = glm::rgb(1), float scale = 10.0f);
	protected:
		std::function<glm::rgb(float u, float v, const glm::vec3 & p)> op;
	};
}

#endif // !_OP_TEXTURE_H_

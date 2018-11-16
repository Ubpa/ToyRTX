#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <RayTracing/Material.h>
#include <RayTracing/Texture.h>

namespace RayTracing {
	class Light : public Material{
		HEAP_OBJ_SETUP(Light)
	public:
		Light(const glm::rgb & color);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(HitRecord & rec) const;
	private:
		Texture::CPtr lightTex;
	};
}

#endif // !_LIGHT_H_

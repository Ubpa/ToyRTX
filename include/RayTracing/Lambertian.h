#ifndef _LAMBERTIAN_H_
#define _LAMBERTIAN_H_

#include <RayTracing/Texture.h>
#include <RayTracing/Material.h>

#include <Utility/Math.h>

namespace RayTracing {
	class Lambertian : public Material {
		MATERIAL_SETUP(Lambertian)
	public:
		Lambertian(float r, float g, float b);
		Lambertian(const glm::rgb & albedo);
		Lambertian(const Texture::CPtr & albedo);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const;
		
		const Texture::CPtr GetTexture() const { return albedo; }
	protected:
		Texture::CPtr albedo;
	};
}

#endif // !_LAMBERTIAN_H_

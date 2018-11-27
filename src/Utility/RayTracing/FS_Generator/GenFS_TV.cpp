#include "GenFS_TV.h"

#include <RayTracing/ConstTexture.h>

using namespace RayTracing;
using namespace glm;
using namespace std;

// TexT : Texture Type
const float TexT_ConstTexture = 0.0f;

void GenFS_TV::Visit(const ConstTexture::CPtr & constTexture) {
	tex2idx[constTexture] = texData.size();
	
	texData.push_back(TexT_ConstTexture);

	const rgb & color = constTexture->GetColor();
	texData.push_back(color.r);
	texData.push_back(color.g);
	texData.push_back(color.b);
}
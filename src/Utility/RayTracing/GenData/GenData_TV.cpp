#include "GenData_TV.h"

#include <RayTracing/ConstTexture.h>
#include <RayTracing/ImgTexture.h>
#include <Utility/Image.h>

using namespace CppUtility::Other;
using namespace RayTracing;
using namespace glm;
using namespace std;

// TexT : Texture Type
const float TexT_ConstTexture = 0.0f;
const float TexT_ImgTexture   = 1.0f;

void GenData_TV::Visit(const ConstTexture::CPtr & constTexture) {
	if (constTexture == NULL)
		return;

	auto targetPair = tex2idx.find(constTexture);
	if (targetPair != tex2idx.end())
		return;

	tex2idx[constTexture] = texData.size();

	texData.push_back(TexT_ConstTexture);

	const rgb & color = constTexture->GetColor();
	texData.push_back(color.r);
	texData.push_back(color.g);
	texData.push_back(color.b);
}

void GenData_TV::Visit(const ImgTexture::CPtr & imgTexture) {
	if (imgTexture == NULL)
		return;

	auto targetTexIdx = tex2idx.find(imgTexture);
	if (targetTexIdx != tex2idx.end())
		return;

	tex2idx[imgTexture] = texData.size();

	texData.push_back(TexT_ImgTexture);

	if (imgTexture->GetImg() == NULL) {
		texData.push_back(-1);
		return;
	}

	auto targetImgIdx = img2idx.find(imgTexture->GetImg());
	if (targetImgIdx == img2idx.end()) {
		size_t idx = img2idx.size();
		img2idx[imgTexture->GetImg()] = idx;
	}

	texData.push_back(img2idx[imgTexture->GetImg()]);
}
#include "GenFS_MV.h"

#include <RayTracing/TexVisitor.h>
#include <RayTracing/Lambertian.h>
#include <RayTracing/Metal.h>
#include <RayTracing/Dielectric.h>

using namespace RayTracing;
using namespace std;

// MatT : Material Type
const float MatT_Lambertian = 0.0f;
const float MatT_Metal = 1.0f;
const float MatT_Dielectric = 2.0f;

const size_t MatSize_Lambertian = 2;
const size_t MatSize_Metal = 3;
const size_t MatSize_Dielectric = 2;

void GenFS_MV::Visit(const Lambertian * lambertian) {
	mat2idx[lambertian] = matData.size();

	matData.push_back(MatT_Lambertian);

	tex2idxVec[lambertian->GetTexture()].push_back(matData.size());
	matData.push_back(-1);
}

void GenFS_MV::Visit(const Metal * metal) {
	mat2idx[metal] = matData.size();

	matData.push_back(MatT_Metal);

	tex2idxVec[metal->GetTexture()].push_back(matData.size());
	matData.push_back(-1);

	matData.push_back(metal->GetFuzz());
}

void GenFS_MV::Visit(const Dielectric * dielectric) {
	mat2idx[dielectric] = matData.size();

	matData.push_back(MatT_Dielectric);

	matData.push_back(dielectric->GetRafractIndex());
}

void GenFS_MV::SetTex(const TexIdxMap & tex2idx) {
	for (auto & pair : tex2idx) {
		auto tmpTexPtr = shared_ptr<const Texture>(pair.first, [](const Texture *) {});
		auto target = tex2idxVec.find(tmpTexPtr);
		if (target != tex2idxVec.end()) {
			for (auto const & idx : target->second)
				matData[idx] = pair.second;
		}
	}
}

void GenFS_MV::Accept(TexVisitor * texVisitor) {
	for (auto const & pair : tex2idxVec)
		pair.first->Accept(texVisitor);
}
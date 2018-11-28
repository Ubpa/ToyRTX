#include "GenData_MV.h"
#include "GenData_TV.h"

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

void GenData_MV::Visit(const Lambertian::CPtr & lambertian) {
	mat2idx[lambertian] = matData.size();

	matData.push_back(MatT_Lambertian);

	if (lambertian->GetTexture())
		tex2idxVec[lambertian->GetTexture()].push_back(matData.size());
	matData.push_back(-1);
}

void GenData_MV::Visit(const Metal::CPtr & metal) {
	mat2idx[metal] = matData.size();

	matData.push_back(MatT_Metal);

	if(metal->GetTexture())
		tex2idxVec[metal->GetTexture()].push_back(matData.size());
	matData.push_back(-1);

	matData.push_back(metal->GetFuzz());
}

void GenData_MV::Visit(const Dielectric::CPtr & dielectric) {
	mat2idx[dielectric] = matData.size();

	matData.push_back(MatT_Dielectric);

	matData.push_back(dielectric->GetRafractIndex());
}

void GenData_MV::SetTex(const TexIdxMap & tex2idx) {
	for (auto & pair : tex2idx) {
		auto target = tex2idxVec.find(pair.first);
		if (target != tex2idxVec.end()) {
			for (auto const & idx : target->second)
				matData[idx] = pair.second;
		}
	}
}

void GenData_MV::Accept(const GenData_TV::Ptr & genFS_TV) {
	for (auto const & pair : tex2idxVec)
		pair.first->Accept(genFS_TV);

	SetTex(genFS_TV->GetTexIdxMap());
}
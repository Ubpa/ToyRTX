#include "GenFS_HV.h"
#include "GenFS_MV.h"

#include <RayTracing/Hitable.h>
#include <RayTracing/Group.h>
#include <RayTracing/Sphere.h>

using namespace RayTracing;
using namespace glm;
using namespace std;

//HT : Hitable Type
const float HT_Sphere = 0.0f;
const float HT_Group = 1.0f;

void GenFS_HV::Visit(const Hitable::CPtr & hitable) {
	if(hitable->GetMat())
		mat2idxVec[hitable->GetMat()].push_back(sceneData.size());
	sceneData.push_back(-1);

	sceneData.push_back(hitable->IsMatCoverable());
}

void GenFS_HV::Visit(const Group::CPtr & group) {
	sceneData.push_back(HT_Group);

	Visit(static_cast<const Hitable::CPtr>(group));

	size_t childrenSize = group->GetChildren().size();
	//sceneData.push_back(childrenSize);

	size_t curChildIt = sceneData.size();
	for (size_t i = 0; i < childrenSize; i++)
		sceneData.push_back(-1);
	
	// 用以标识结尾
	sceneData.push_back(-1);

	for (auto const & child : group->GetChildren()) {
		sceneData[curChildIt++] = sceneData.size();
		child->Accept(This());
	}
}

void GenFS_HV::Visit(const Sphere::CPtr & sphere) {
	sceneData.push_back(HT_Sphere);

	Visit(static_cast<const Hitable::CPtr>(sphere));

	vec3 center = sphere->GetCenter();
	sceneData.push_back(center.x);
	sceneData.push_back(center.y);
	sceneData.push_back(center.z);

	sceneData.push_back(sphere->GetRadius());
}

void GenFS_HV::SetMat(const MatIdxMap & mat2idx) {
	for (auto const & pair : mat2idx) {
		auto target = mat2idxVec.find(pair.first);
		if (target != mat2idxVec.end()) {
			for (auto const & idx : target->second)
				sceneData[idx] = pair.second;
		}
	}
}

void GenFS_HV::Accept(const GenFS_MV::Ptr & genFS_MV) {
	for (auto const & pair : mat2idxVec)
		pair.first->Accept(genFS_MV);

	SetMat(genFS_MV->GetMatIdxMap());
}
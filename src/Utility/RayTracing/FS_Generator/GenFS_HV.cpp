#include "GenFS_HV.h"

#include <RayTracing/MatVisitor.h>
#include <RayTracing/Group.h>
#include <RayTracing/Sky.h>
#include <RayTracing/Sphere.h>

using namespace RayTracing;
using namespace glm;
using namespace std;

//HT : Hitable Type
const float HT_Sphere = 0.0f;
const float HT_Group = 1.0f;

void GenFS_HV::Visit(const Group * group) {
	sceneData.push_back(HT_Group);

	mat2idxVec[group->GetMat()].push_back(sceneData.size());
	sceneData.push_back(-1);

	size_t childrenSize = group->GetChildren().size();
	sceneData.push_back(childrenSize);

	size_t curChildIt = sceneData.size();
	for (size_t i = 0; i < childrenSize; i++)
		sceneData.push_back(-1);

	for (auto const & child : group->GetChildren()) {
		sceneData[curChildIt++] = sceneData.size();
		child->Accept(this);
	}
}

void GenFS_HV::Visit(const Sphere * sphere) {
	sceneData.push_back(HT_Sphere);

	mat2idxVec[sphere->GetMat()].push_back(sceneData.size());
	sceneData.push_back(-1);

	vec3 center = sphere->GetCenter();
	sceneData.push_back(center.x);
	sceneData.push_back(center.y);
	sceneData.push_back(center.z);

	sceneData.push_back(sphere->GetRadius());
}

void GenFS_HV::SetMat(const MatIdxMap & mat2idx) {
	for (auto const & pair : mat2idx) {
		auto tmpMatPtr = shared_ptr<const Material>(pair.first, [](const Material *) {});
		auto target = mat2idxVec.find(tmpMatPtr);
		if (target != mat2idxVec.end()) {
			for (auto const & idx : target->second)
				sceneData[idx] = pair.second;
		}
	}
}

void GenFS_HV::Accept(MatVisitor * matVisitor) {
	for (auto const & pair : mat2idxVec)
		pair.first->Accept(matVisitor);
}
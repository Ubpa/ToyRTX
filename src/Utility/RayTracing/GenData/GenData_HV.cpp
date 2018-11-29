#include "GenData_HV.h"
#include "GenData_MV.h"

#include <RayTracing/Volume.h>
#include <RayTracing/Group.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/BVH_Node.h>
#include <RayTracing/Triangle.h>
#include <RayTracing/TriMesh.h>
#include <RayTracing/Transform.h>

using namespace RayTracing;
using namespace glm;
using namespace std;

//HT : Hitable Type
const float HT_Sphere    = 0.0f;
const float HT_Group     = 1.0f;
const float HT_BVH_Node  = 2.0f;
const float HT_Triangle  = 3.0f;
const float HT_TriMesh   = 4.0f;
const float HT_Transform = 5.0f;
const float HT_Volume    = 6.0f;

void GenData_HV::Visit(const Hitable::CPtr & hitable) {
	if (hitable == NULL)
		return;

	if(hitable->GetMat())
		mat2idxVec[hitable->GetMat()].push_back(sceneData.size());
	sceneData.push_back(-1);

	sceneData.push_back(hitable->IsMatCoverable());

	const AABB box = hitable->GetBoundingBox();
	sceneData.push_back(box.GetMinP().x);
	sceneData.push_back(box.GetMinP().y);
	sceneData.push_back(box.GetMinP().z);
	sceneData.push_back(box.GetMaxP().x);
	sceneData.push_back(box.GetMaxP().y);
	sceneData.push_back(box.GetMaxP().z);
}

void GenData_HV::Visit(const Sphere::CPtr & sphere) {
	if (sphere == NULL)
		return;

	auto targetPair = hitable2idx.find(sphere);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[sphere] = sceneData.size();

	sceneData.push_back(HT_Sphere);

	Visit(static_cast<const Hitable::CPtr>(sphere));

	vec3 center = sphere->GetCenter();
	sceneData.push_back(center.x);
	sceneData.push_back(center.y);
	sceneData.push_back(center.z);

	sceneData.push_back(sphere->GetRadius());
}

void GenData_HV::Visit(const Group::CPtr & group) {
	if (group == NULL)
		return;

	auto targetPair = hitable2idx.find(group);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[group] = sceneData.size();

	sceneData.push_back(HT_Group);

	Visit(static_cast<const Hitable::CPtr>(group));

	size_t childrenSize = group->GetChildren().size();
	//sceneData.push_back(childrenSize);

	size_t curChildIt = sceneData.size();
	for (size_t i = 0; i < childrenSize; i++)
		sceneData.push_back(-1);
	
	// childrenEnd 用以标识结尾
	sceneData.push_back(-1);

	for (auto const & child : group->GetChildren()) {
		if (child == NULL)
			continue;

		auto targetHitableIdx = hitable2idx.find(child);
		if (targetHitableIdx != hitable2idx.end()) {
			sceneData[curChildIt++] = targetHitableIdx->second;
			continue;
		}

		sceneData[curChildIt++] = sceneData.size();
		child->Accept(This());
	}
}

void GenData_HV::Visit(const BVH_Node::CPtr & bvhNode) {
	if (bvhNode == NULL)
		return;

	auto targetPair = hitable2idx.find(bvhNode);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[bvhNode] = sceneData.size();

	sceneData.push_back(HT_BVH_Node);
	Visit(static_cast<const Hitable::CPtr>(bvhNode));

	size_t curChildIt = sceneData.size();
	// left
	if(bvhNode->GetLeft())
		sceneData.push_back(-1);
	// right
	if(bvhNode->GetRight())
		sceneData.push_back(-1);
	
	// childrenEnd 用以标识结尾
	sceneData.push_back(-1);

	if (bvhNode->GetLeft()) {
		auto targetLeftIdx = hitable2idx.find(bvhNode->GetLeft());
		if (targetLeftIdx == hitable2idx.end()) {
			sceneData[curChildIt++] = sceneData.size();
			bvhNode->GetLeft()->Accept(This());
		}else
			sceneData[curChildIt++] = targetLeftIdx->second;
	}

	if (bvhNode->GetRight()) {
		auto targetRightIdx = hitable2idx.find(bvhNode->GetRight());
		if (targetRightIdx == hitable2idx.end()) {
			sceneData[curChildIt++] = sceneData.size();
			bvhNode->GetRight()->Accept(This());
		}
		else
			sceneData[curChildIt++] = targetRightIdx->second;
	}
}

void GenData_HV::Visit(const Triangle::CPtr & triangle) {
	if (triangle == NULL)
		return;

	auto targetPair = hitable2idx.find(triangle);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[triangle] = sceneData.size();

	sceneData.push_back(HT_Triangle);

	Visit(static_cast<Hitable::CPtr>(triangle));

	const Vertex ABC[3] = { triangle->GetA(),triangle->GetB(),triangle->GetC() };
	for (size_t i = 0; i < 3; i++) {
		sceneData.push_back(ABC[i].pos.x);
		sceneData.push_back(ABC[i].pos.y);
		sceneData.push_back(ABC[i].pos.z);
		sceneData.push_back(ABC[i].normal.x);
		sceneData.push_back(ABC[i].normal.y);
		sceneData.push_back(ABC[i].normal.z);
		sceneData.push_back(ABC[i].u);
		sceneData.push_back(ABC[i].v);
	}
}

void GenData_HV::Visit(const TriMesh::CPtr & triMesh) {
	if (triMesh == NULL)
		return;

	auto targetPair = hitable2idx.find(triMesh);
	if (targetPair != hitable2idx.end())
		return;

	Visit(static_cast<const BVH_Node::CPtr>(triMesh));
	sceneData[hitable2idx[triMesh]] = HT_TriMesh;
}

void GenData_HV::Visit(const Transform::CPtr & transform) {
	if (transform == NULL)
		return;

	auto targetPair = hitable2idx.find(transform);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[transform] = sceneData.size();

	sceneData.push_back(HT_Transform);

	Visit(static_cast<Hitable::CPtr>(transform));

	mat4 tfmMat4 = transform->GetTransform();
	for (size_t col = 0; col < 4; col++) {
		vec4 colVec4 = tfmMat4[col];
		for (size_t row = 0; row < 4; row++)
			sceneData.push_back(colVec4[row]);
	}

	mat4 invTfmMat4 = transform->GetInvTransform();
	for (size_t col = 0; col < 4; col++) {
		vec4 colVec4 = invTfmMat4[col];
		for (size_t row = 0; row < 4; row++)
			sceneData.push_back(colVec4[row]);
	}

	mat3 normTfmMat3 = transform->GetNormTransform();
	for (size_t col = 0; col < 3; col++) {
		vec3 colVec3 = normTfmMat3[col];
		for (size_t row = 0; row < 3; row++)
			sceneData.push_back(colVec3[row]);
	}

	if (transform->GetChild() == NULL) {
		sceneData.push_back(-1);
		return;
	}

	size_t childIt = sceneData.size();
	sceneData.push_back(-1);
	sceneData.push_back(-1);

	auto targetChildIdx = hitable2idx.find(transform->GetChild());
	if (targetChildIdx == hitable2idx.end()) {
		sceneData[childIt] = sceneData.size();
		transform->GetChild()->Accept(This());
	}
	else
		sceneData[childIt] = targetChildIdx->second;
}

void GenData_HV::Visit(const Volume::CPtr & volume) {
	if (volume == NULL)
		return;

	auto targetPair = hitable2idx.find(volume);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[volume] = sceneData.size();

	sceneData.push_back(HT_Volume);

	Visit(static_cast<Hitable::CPtr>(volume));

	sceneData.push_back(volume->GetDensity());

	if (volume->GetBoundary() == NULL) {
		sceneData.push_back(-1);
		return;
	}

	size_t childIt = sceneData.size();
	sceneData.push_back(-1);
	sceneData.push_back(-1);

	auto targetChildIdx = hitable2idx.find(volume->GetBoundary());
	if (targetChildIdx == hitable2idx.end()) {
		sceneData[childIt] = sceneData.size();
		volume->GetBoundary()->Accept(This());
	}
	else
		sceneData[childIt] = targetChildIdx->second;
}

void GenData_HV::SetMat(const MatIdxMap & mat2idx) {
	for (auto const & pair : mat2idx) {
		auto target = mat2idxVec.find(pair.first);
		if (target != mat2idxVec.end()) {
			for (auto const & idx : target->second)
				sceneData[idx] = pair.second;
		}
	}
}

void GenData_HV::Accept(const GenData_MV::Ptr & genFS_MV) {
	for (auto const & pair : mat2idxVec)
		pair.first->Accept(genFS_MV);

	SetMat(genFS_MV->GetMatIdxMap());
}
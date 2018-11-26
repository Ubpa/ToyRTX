#include "GenFS_HV.h"

#include <RayTracing/Group.h>
#include <RayTracing/Sphere.h>
#include <RayTracing/Sky.h>

using namespace RayTracing;
using namespace glm;

//HT : Hitable Type
const float HT_Sphere = 0.0f;
const float HT_Sky = 1.0f;
const float HT_Group = 2.0f;

void GenFS_HV::Visit(const Group * group) {

}

void GenFS_HV::Visit(const Sphere * sphere) {
	sceneData.push_back(HT_Sphere);

	auto target = mat2idx.find(sphere->GetMat());
	if (target == mat2idx.end()) {
		size_t newIdx = mat2idx.size();
		mat2idx[sphere->GetMat()] = newIdx;
		sceneData.push_back(newIdx);
	}else
		sceneData.push_back(mat2idx[sphere->GetMat()]);

	vec3 center = 
	sceneData.push_back()
}

void GenFS_HV::Visit(const Sky * sky) {

}
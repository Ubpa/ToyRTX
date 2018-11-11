#include <RayTracing/Scene.h>

using namespace RayTracing;
using namespace CppUtility::Other;

Scene::Scene(const Hitable::Ptr & obj, const RayCamera::Ptr & camera)
	: obj(obj), camera(camera) { }
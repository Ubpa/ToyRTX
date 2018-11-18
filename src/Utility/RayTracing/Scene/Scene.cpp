#include <RayTracing/Scene.h>

using namespace RayTracing;
using namespace CppUtility::Other;

Scene::Scene(const Hitable::CPtr & obj, const RayCamera::CPtr & camera)
	: obj(obj), camera(camera) { }
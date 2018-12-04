#include <RayTracing/Hitable.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;
using namespace std;

const HitRst HitRst::InValid(false);

Hitable::Hitable(Material::CPtr material)
	: material(material) {
	isMatCoverable = material ? false : true;
}
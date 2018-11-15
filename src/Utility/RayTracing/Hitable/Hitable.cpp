#include <RayTracing/Hitable.h>
#include <Utility/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;
using namespace std;

const HitRst HitRst::FALSE(false);

Hitable::Hitable(const Material::Ptr & material)
	: material(material) {
	isMatCoverable = material ? false : true;
}
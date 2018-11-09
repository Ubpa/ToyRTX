#include <Utility/Math.h>
#include <random>

using namespace CppUtility::Other;
using namespace glm;
using namespace std;

static uniform_int_distribution<unsigned> uiMap;
static uniform_int_distribution<signed> iMap;
static uniform_real_distribution<float> fMap(0.0f,1.0f);
static uniform_real_distribution<double> dMap(0.0,1.0);
static default_random_engine engine;

namespace CppUtility {
	namespace Other {
		int Rand_I() {
			return iMap(engine);
		}

		unsigned int Rand_UI() {
			return uiMap(engine);
		}

		float Rand_F() {
			return fMap(engine);
		}

		double Rand_D() {
			return dMap(engine);
		}

		vec2 RandInCircle() {
			vec2 rst;
			do {
				rst = vec2(fMap(engine), fMap(engine))*2.0f - 1.0f;
			} while (dot(rst, rst) >= 1.0);
			return rst;
		}

		glm::vec3 RandInSphere() {
			vec3 rst;
			do {
				rst = vec3(fMap(engine), fMap(engine), fMap(engine))*2.0f - 1.0f;
			} while (dot(rst, rst) >= 1.0);
			return rst;
		}

	}
}
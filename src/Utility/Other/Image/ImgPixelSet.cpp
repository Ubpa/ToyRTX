#include <Utility/ImgPixelSet.h>

using namespace CppUtility::Other;
using namespace glm;

ImgPixelSet::ImgPixelSet() {

}

ImgPixelSet::ImgPixelSet(size_t width, size_t height) {
	data.reserve(width * height);
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			Insert(glm::vec2(i, j));
		}
	}
}

size_t ImgPixelSet::RandPick(size_t n, std::vector<glm::vec2> & pixels) {
	pixels.clear();
	const size_t num = glm::min(Size(), n);
	pixels.reserve(num);
	for (size_t i=0; i < num; i++)
		pixels.push_back(RandSet<vec2>::RandPick());
	return num;
}


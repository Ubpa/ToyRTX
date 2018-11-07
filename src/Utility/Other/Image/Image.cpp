#include <Utility/Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Utility/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Utility/stb_image_write.h>

#include <Utility/File.h>

using namespace CppUtility::Other;
using namespace std;

Image::Pixel<uByte> Image::Pixel_F2UB(const Pixel<float> & pixel) {
	Image::Pixel<uByte> rst(pixel.channel);
	for (size_t i = 0; i < pixel.channel; i++) {
		float mappedVal = 255.99f * pixel[i];
		rst[i] = static_cast<uByte>(mappedVal >= 256.0f ? 255 : static_cast<uByte>(mappedVal));
	}

	return rst;
}

Image::Pixel<uByte> Image::Pixel_D2UB(const Pixel<double> & pixel) {
	Image::Pixel<uByte> rst(pixel.channel);
	for (size_t i = 0; i < pixel.channel; i++) {
		double mappedVal = 255.99 * pixel[i];
		rst[i] = static_cast<uByte>(mappedVal >= 256.0 ? 255 : static_cast<uByte>(mappedVal));
	}

	return rst;
}

Image::Pixel<float> Image::Pixel_UB2F(const Pixel<uByte> & pixel) {
	Image::Pixel<float> rst(pixel.channel);
	for (size_t i = 0; i < pixel.channel; i++)
		rst[i] = 1.0f / 255.0f * pixel[i];

	return rst;
}

Image::Pixel<double> Image::Pixel_UB2D(const Pixel<uByte> & pixel) {
	Image::Pixel<double> rst(pixel.channel);
	for (size_t i = 0; i < pixel.channel; i++)
		rst[i] = 1.0 / 255.0 * pixel[i];

	return rst;
}

//------------

Image::Image()
	:data(NULL), width(0), height(0), channel(0), type(ENUM_SRC_TYPE_INVALID){ }

Image::Image(int width, int height, int channel){
	GenBuffer(width, height, channel);
}

Image::Image(const char * fileName, bool flip, int req_comp) {
	data = NULL;
	type = ENUM_SRC_TYPE_INVALID;
	Load(fileName, flip, req_comp);
}

Image::~Image() {
	Free();
}

//-----------

bool Image::IsValid() const{
	return data != NULL && type != ENUM_SRC_TYPE_INVALID;
}

uByte * Image::GetData() {
	if (!IsValid())
		return NULL;

	return data;
}

const uByte * Image::GetConstData() const{
	if (!IsValid())
		return NULL;

	return data;
}

int Image::GetWidth() const{
	return width;
}

int Image::GetHeight() const{
	return height;
}

int Image::GetChannel() const{
	return channel;
}

//------------

uByte & Image::At(size_t x, size_t y, size_t channel) {
	return data[(y*width + x)*this->channel + channel];
}

bool Image::SetPixel(size_t x, size_t y, const Pixel<uByte> & pixel) {
	if (pixel.channel != this->channel)
		return false;

	for (size_t i = 0; i < channel; i++)
		At(x, y, i) = pixel[i];

	return true;
}

bool Image::SetPixel(size_t x, size_t y, const Image::Pixel<float> & pixel) {
	return SetPixel(x, y, Pixel_F2UB(pixel));
}

bool Image::SetPixel(size_t x, size_t y, const Image::Pixel<double> & pixel) {
	return SetPixel(x, y, Pixel_D2UB(pixel));
}

Image::Pixel<uByte> Image::GetPixel_UB(size_t x, size_t y) {
	if (channel == 3)
		return { At(x, y, 0), At(x, y, 1), At(x, y, 2) };
	else if (channel == 4)
		return { At(x, y, 0), At(x, y, 1), At(x, y, 2), At(x, y, 3) };
}


Image::Pixel<float> Image::GetPixel_F(size_t x, size_t y) {
	return Pixel_UB2F(GetPixel_UB(x, y));
}

Image::Pixel<double> Image::GetPixel_D(size_t x, size_t y) {
	return Pixel_UB2D(GetPixel_UB(x, y));
}

//------------

bool Image::Load(const std::string & fileName, bool flip, int req_comp) {
	Free();
	stbi_set_flip_vertically_on_load(flip);
	data = stbi_load(fileName.c_str(), &width, &height, &channel, req_comp);
	if (data == NULL) {
		type = ENUM_SRC_TYPE_INVALID;
		return false;
	}

	type = ENUM_SRC_TYPE_STB;
	return true;
}
void Image::GenBuffer(int width, int height, int channel) {
	Free();
	this->width = width;
	this->height = height;
	this->channel = channel;

	data = new uByte[width*height*channel];
	type = ENUM_SRC_TYPE_NEW;
}

void Image::Free() {
	if (data != NULL) {
		switch (type)
		{
		case Other::Image::ENUM_SRC_TYPE_NEW:
			delete[] data;
			break;
		case Other::Image::ENUM_SRC_TYPE_STB:
			stbi_image_free(data);
			break;
		case Other::Image::ENUM_SRC_TYPE_INVALID:
		default:
			break;
		}
	}

	width = 0;
	height = 0;
	channel = 0;
	data = NULL;
	type = ENUM_SRC_TYPE_INVALID;
}

bool Image::SaveAsPNG(const string & fileName, bool flip) {
	stbi_flip_vertically_on_write(flip);
	return stbi_write_png(fileName.c_str(), width, height, channel, data, width * 3);
}
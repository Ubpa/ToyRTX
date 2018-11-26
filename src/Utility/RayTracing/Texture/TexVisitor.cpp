#include <RayTracing/TexVisitor.h>

using namespace RayTracing;

void Visit(const ConstTexture * constTexture) { }
void Visit(const OpTexture * opTexture) { }
void Visit(const ImgTexture * imgTexture) { }
#include <RayTracing/TexVisitor.h>

using namespace RayTracing;

void TexVisitor::Visit(const ConstTexture * constTexture) { }
void TexVisitor::Visit(const OpTexture * opTexture) { }
void TexVisitor::Visit(const ImgTexture * imgTexture) { }
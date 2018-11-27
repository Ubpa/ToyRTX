#include <RayTracing/MatVisitor.h>

using namespace RayTracing;
using namespace CppUtility;

void MatVisitor::Visit(const Other::CPtr<Lambertian> & Lambertian) { }
void MatVisitor::Visit(const Other::CPtr<Metal> & Metal) { }
void MatVisitor::Visit(const Other::CPtr<Dielectric> & Dielectric) { }
void MatVisitor::Visit(const Other::CPtr<Light> & Light) { }
void MatVisitor::Visit(const Other::CPtr<OpMaterial> & OpMaterial) { }
void MatVisitor::Visit(const Other::CPtr<Isotropic> & Isotropic) { }
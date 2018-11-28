#include <RayTracing/MatVisitor.h>

using namespace RayTracing;
using namespace CppUtility;

void MatVisitor::Visit(const Other::CPtr<Material> & material) { }
void MatVisitor::Visit(const Other::CPtr<Lambertian> & lambertian) { }
void MatVisitor::Visit(const Other::CPtr<Metal> & metal) { }
void MatVisitor::Visit(const Other::CPtr<Dielectric> & dielectric) { }
void MatVisitor::Visit(const Other::CPtr<Light> & light) { }
void MatVisitor::Visit(const Other::CPtr<OpMaterial> & opMaterial) { }
void MatVisitor::Visit(const Other::CPtr<Isotropic> & isotropic) { }
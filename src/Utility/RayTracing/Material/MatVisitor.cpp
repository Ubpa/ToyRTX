#include <RayTracing/MatVisitor.h>

using namespace RayTracing;

void MatVisitor::Visit(const Lambertian * Lambertian) { }
void MatVisitor::Visit(const Metal * Metal) { }
void MatVisitor::Visit(const Dielectric * Dielectric) { }
void MatVisitor::Visit(const Light * Light) { }
void MatVisitor::Visit(const OpMaterial * OpMaterial) { }
void MatVisitor::Visit(const Isotropic * Isotropic) { }
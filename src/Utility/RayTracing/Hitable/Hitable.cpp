#include <RayTracing/Hitable.h>

using namespace RayTracing;

Hitable::~Hitable() { }

void Hitable::ProtectedDelete(Hitable * op) { delete op; }
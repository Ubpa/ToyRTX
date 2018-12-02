#include <RayTracing/HitableVisitor.h>

using namespace CppUtility;
using namespace RayTracing;

void HitableVisitor::Visit(const Other::CPtr<Hitable> & hitable) { }
void HitableVisitor::Visit(const Other::CPtr<Sphere> & sphere) { }
void HitableVisitor::Visit(const Other::CPtr<Sky> & sky) { }
void HitableVisitor::Visit(const Other::CPtr<MoveSphere> & moveSphere) { }
void HitableVisitor::Visit(const Other::CPtr<Triangle> & triangle) { }
void HitableVisitor::Visit(const Other::CPtr<Transform> & transform) { }
void HitableVisitor::Visit(const Other::CPtr<Volume> & volume) { }
void HitableVisitor::Visit(const Other::CPtr<Group> & group) { }
void HitableVisitor::Visit(const Other::CPtr<BVH_Node> & bvhNode) { }
void HitableVisitor::Visit(const Other::CPtr<TriMesh> & triMesh) { }
void HitableVisitor::Visit(const Other::CPtr<Model> & model) { }
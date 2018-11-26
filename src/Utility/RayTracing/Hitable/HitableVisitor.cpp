#include <RayTracing/HitableVisitor.h>

using namespace RayTracing;

void HitableVisitor::Visit(const Sphere * sphere){ }
void HitableVisitor::Visit(const Sky * sky){ }
void HitableVisitor::Visit(const MoveSphere * moveSphere){ }
void HitableVisitor::Visit(const Triangle * triangle){ }
void HitableVisitor::Visit(const Transform * transform){ }
void HitableVisitor::Visit(const Volume * volume){ }
void HitableVisitor::Visit(const Group * group){ }
void HitableVisitor::Visit(const BVH_Node * bvhNode){ }
void HitableVisitor::Visit(const TriMesh * triMesh){ }
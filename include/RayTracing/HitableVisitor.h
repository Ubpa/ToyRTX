#ifndef _HITABLE_VISITOR_H_
#define _HITABLE_VISITOR_H_

#include <Utility/HeapObj.h>

namespace RayTracing {
	class Sphere;
	class Sky;
	class MoveSphere;
	class Triangle;
	class Transform;
	class Volume;
	class Group;
	class BVH_Node;
	class TriMesh;

	class HitableVisitor : public CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(HitableVisitor)
	public:
		virtual void Visit(const Sphere * sphere);
		virtual void Visit(const Sky * sky);
		virtual void Visit(const MoveSphere * moveSphere);
		virtual void Visit(const Triangle * triangle);
		virtual void Visit(const Transform * transform);
		virtual void Visit(const Volume * volume);
		virtual void Visit(const Group * group);
		virtual void Visit(const BVH_Node * bvhNode);
		virtual void Visit(const TriMesh * triMesh);
	};
}

#endif // !_HITABLE_VISITOR_H_

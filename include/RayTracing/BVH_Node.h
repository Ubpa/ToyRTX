#ifndef _BVH_NODE_H_
#define _BVH_NODE_H_

#include <RayTracing/Hitable.h>
#include <vector>

namespace RayTracing {
	class BVH_Node : public Hitable{
		HEAP_OBJ_SETUP(BVH_Node)
	public:
		BVH_Node() : box(AABB::InValid){ }
		BVH_Node(std::vector<Hitable::Ptr> & hitables);

		virtual HitRst RayIn(Ray::Ptr & ray) const;
		virtual AABB GetBoundingBox() const { return box; };

	protected:
		void Build(std::vector<Hitable::Ptr>::iterator begin, std::vector<Hitable::Ptr>::iterator end);
		size_t GetAxis(std::vector<Hitable::Ptr>::const_iterator begin, const std::vector<Hitable::Ptr>::const_iterator end) const;
	private:
		Hitable::Ptr left;
		Hitable::Ptr right;
		AABB box;
	};
}

#endif // !_BVH_NODE_H_

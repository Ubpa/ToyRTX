#ifndef _TRI_MESH_H_
#define _TRI_MESH_H_

#include <RayTracing/BVH_Node.h>
#include <RayTracing/Triangle.h>

#include <vector>

namespace RayTracing {
	class TriMesh : public BVH_Node{
		HEAP_OBJ_SETUP(TriMesh)
	public:
		TriMesh(const std::vector<Vertex> & vertexs, const Material::Ptr & material = NULL);

		bool IsValid() const { return isValid; };
	protected:
		bool isValid;
	};
}

#endif // !_TRI_MESH_H_

#ifndef _TRI_MESH_H_
#define _TRI_MESH_H_

#include <RayTracing/BVH_Node.h>

#include <vector>

namespace RayTracing {
	class TriMesh : public BVH_Node{
		HITABLE_SETUP(TriMesh)
	public:
		TriMesh(const std::vector<Vertex> & vertexs, const Material::CPtr & material = NULL);

		inline bool IsValid() const { return isValid; };
	protected:
		bool isValid;
	};
}

#endif // !_TRI_MESH_H_

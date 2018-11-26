#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <RayTracing/Ray.h>

#include <Utility/HeapObj.h>

#include <glm/glm.hpp>

#include <vector>

#include <RayTracing/MatVisitor.h>

#define MATERIAL_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(const MatVisitor::Ptr & matVisitor) const{\
	matVisitor->Visit(this);\
}

namespace RayTracing {
	
	struct Vertex {
		Vertex(glm::vec3 pos = glm::vec3(0), glm::vec3 normal = glm::vec3(0, 0, 1), float u = 0, float v = 0);
		glm::vec3 pos;
		glm::vec3 normal;
		float u;
		float v;
		
		void Transform(const glm::mat4 & transform);
		void Transform(const glm::mat4 & transform, const glm::mat3 & normalTransform);
		static const Vertex Interpolate_Tri(const glm::vec3 & abg, const Vertex & A, const Vertex & B, const Vertex & C);
	};

	struct HitRecord {
		HitRecord(const Ray::Ptr & ray = NULL, const glm::vec3 & pos = glm::vec3(0), const glm::vec3 & normal = glm::vec3(0,0,1), float u = 0, float v = 0);

		Ray::Ptr ray;
		Vertex vertex;
	};

	class Material : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Material)
	public:
		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const = 0;
	};
}

#endif // !_MATERIAL_H_

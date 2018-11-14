#include <RayTracing/Material.h>

using namespace RayTracing;
using namespace glm;

Vertex::Vertex(vec3 pos, vec3 normal, float u, float v) 
	: pos(pos), normal(normal), u(u), v(v){ }

Vertex Vertex::Interpolate_Tri(const glm::vec3 & abg, const Vertex & A, const Vertex & B, const Vertex & C) {
	Vertex rst;

	rst.u = dot(vec3(abg), vec3(A.u, B.u, C.u));
	rst.v = dot(vec3(abg), vec3(A.v, B.v, C.v));
	rst.pos = abg[0] * A.pos + abg[1] * B.pos + abg[2] * C.pos;
	rst.normal = abg[0] * A.normal + abg[1] * B.normal + abg[2] * C.normal;

	return rst;
}

HitRecord::HitRecord(const Ray::Ptr & ray, const vec3 & pos, const vec3 & normal, float u, float v)
	:ray(ray), vertex(pos,normal,u,v) { }
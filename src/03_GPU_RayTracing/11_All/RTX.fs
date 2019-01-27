#version 330 core

layout (location = 0) out vec4 out_origin_curRayNum;
layout (location = 1) out vec4 out_dir_tMax;
layout (location = 2) out vec4 out_color_time;
layout (location = 3) out vec3 out_rayTracingRst;

struct Ray{
    vec3 origin;
    vec3 dir;
    highp vec3 color;
    float tMax;
    float time;
};
float curRayNum;
void Ray_Update(inout struct Ray ray, vec3 origin, vec3 dir, vec3 attenuation);
void Ray_Transform(inout struct Ray ray, mat4 transform);

struct Camera{
    vec3 pos;
    vec3 BL_Corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 right;
    vec3 up;
    vec3 front;
    float lenR;
    float t0;
    float t1;
};
void Camera_GenRay(out struct Ray ray);

struct Vertex{
    vec3 pos;
    vec3 normal;
    vec2 uv;
};
struct Vertex Vertex_InValid = struct Vertex(vec3(0), vec3(0), vec2(0));
void Vertex_Load(float idx, out struct Vertex vert);
void Vertex_Interpolate(vec3 abg, struct Vertex A, struct Vertex B, struct Vertex C, out struct Vertex vert);
void Vertex_Transform(inout struct Vertex vert, mat4 transform);
void Vertex_Transform(inout struct Vertex vert, mat4 transform , mat3 normTfm);

struct HitRst{
    bool hit;
    struct Vertex vertex;
    float matIdx;
	float isMatCoverable;
};
struct HitRst HitRst_InValid = struct HitRst(false, Vertex_InValid, -1, 0);

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;//若tMax为0, 则表示该光线无效
uniform sampler2D color_time;
uniform sampler2D RTXRst;

uniform sampler2D SceneData;
uniform sampler2D MatData;
uniform sampler2D TexData;
uniform sampler2D PackData;

uniform sampler2D TexArr[16];

uniform struct Camera camera;
uniform float rdSeed[4];
uniform float RayNumMax;
//uniform bool enableGammaCorrection;
#define ENABLE_GAMMA_CORRECTION 0

const float PI = 3.1415926;
const float tMin = 0.001;
const float FLT_MAX = 99999999999999999999999999999999999999.0;
const float rayP = 50.0/51.0;// depth = p/(1-p) --> p = depth/(depth+1)
const float HT_Sphere         = 0.0;
const float HT_Group          = 1.0;
const float HT_BVH_Node       = 2.0;
const float HT_Triangle       = 3.0;
const float HT_TriMesh        = 4.0;
const float HT_Transform      = 5.0;
const float HT_Volume         = 6.0;
const float MatT_Lambertian   = 0.0;
const float MatT_Metal        = 1.0;
const float MatT_Dielectric   = 2.0;
const float MatT_Light        = 3.0;
const float MatT_Isotropic    = 4.0;
const float TexT_ConstTexture = 0.0;
const float TexT_ImgTexture   = 1.0;

int rdCnt = 0;
in vec2 TexCoords;
//struct Ray ray;

float _Stack[50];
int _Stack_mTop = -1;
bool Stack_Empty();
void Stack_Clear();
float Stack_Top();
void Stack_Push(bool val);
void Stack_Push(float val);
void Stack_Push(vec2 v2);
void Stack_Push(vec3 v3);
void Stack_Push(struct Ray ray);
void Stack_Push(struct Vertex vert);
void Stack_Push(struct HitRst hitRst);
float Stack_Pop(); 
void Stack_Pop(out bool val); 
void Stack_Pop(out float val); 
void Stack_Pop(out vec2 v2); 
void Stack_Pop(out vec3 v3); 
void Stack_Pop(out struct Ray ray); 
void Stack_Pop(out struct Vertex vert); 
void Stack_Pop(out struct HitRst hitRst);

float RandXY(float x, float y);// [0.0, 1.0)
float Rand();// [0.0, 1.0)
vec2 RandInSquare();
vec2 RandInCircle();
vec3 RandInSphere();
float At(sampler2D data, float idx);
void GetPack(float idx, out vec4 pack);
void GetPack(float idx, out vec3 pack);
void LoadMat(float idx, out mat4 m);
void LoadMat(float idx, out mat3 m);
float atan2(float y, float x);
vec2 Sphere2UV(vec3 normal);
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);
vec4 Intersect_RayTri(vec3 e, vec3 d, vec3 a, vec3 b, vec3 c);
bool GetRay(out struct Ray ray);
void WriteRay(struct Ray ray, int mode);
void RayIn_Scene(inout struct Ray ray, out struct HitRst finalHitRst);
void RayIn_Sphere(float idx, inout struct Ray ray, inout struct HitRst hitRst);
void RayIn_Triangle(float idx, inout struct Ray ray, inout struct HitRst hitRst);
void RayIn_Volume(float idx, float state, inout struct Ray ray, inout struct HitRst hitRst);
bool AABB_Hit(struct Ray ray, float idx);
bool Scatter_Material(inout struct Ray ray, struct Vertex vertex, float matIdx);
bool Scatter_Lambertian(inout struct Ray ray, struct Vertex vertex, float matIdx);
bool Scatter_Metal(inout struct Ray ray, struct Vertex vertex, float matIdx);
bool Scatter_Dielectric(inout struct Ray ray, struct Vertex vertex, float matIdx);
bool Scatter_Light(inout struct Ray ray, struct Vertex vertex, float matIdx);
bool Scatter_Isotropic(inout struct Ray ray, struct Vertex vertex, float matIdx);
vec3 Value_Texture(vec2 uv, vec3 p, float texIdx);
vec3 Value_ConstTexture(float texIdx);
vec3 Value_ImgTexture(vec2 uv, float texIdx);
void RayTracer();

void main(){
	//if(TexCoords.y<0.33)
	//	out_rayTracingRst=vec3(At(TexData, TexCoords.x * textureSize(TexData, 0).x),0,0);
	//else if(TexCoords.y<0.67)
	//	out_rayTracingRst=vec3(At(MatData, TexCoords.x * textureSize(MatData, 0).x),0,0);
	//else
	//	out_rayTracingRst=vec3(At(SceneData, TexCoords.x * textureSize(SceneData, 0).x),0,0);
	RayTracer();
}

void Vertex_Load(float idx, out struct Vertex vert){
	vec4 pos_u, normal_v;
	GetPack(idx, pos_u);
	GetPack(idx+1, normal_v);

	vert.pos    = pos_u.xyz;
	vert.normal = normal_v.xyz;
	vert.uv     = vec2(pos_u[3], normal_v[3]);
}

void Vertex_Interpolate(vec3 abg, struct Vertex A, struct Vertex B, struct Vertex C, out struct Vertex vert){
	vert.uv[0] = dot(abg, vec3(A.uv[0], B.uv[0], C.uv[0]));
	vert.uv[1] = dot(abg, vec3(A.uv[1], B.uv[1], C.uv[1]));
	vert.pos = abg[0] * A.pos + abg[1] * B.pos + abg[2] * C.pos;
	vert.normal = abg[0] * A.normal + abg[1] * B.normal + abg[2] * C.normal;
}

void Vertex_Transform(inout struct Vertex vert, mat4 transform){
	vec4 posQ = transform * vec4(vert.pos, 1.0);
	vert.pos = posQ.xyz / posQ.w;
	vert.normal = normalize(transpose(inverse(mat3(transform))) * vert.normal);
}

void Vertex_Transform(inout struct Vertex vert, mat4 transform , mat3 normTfm){
	vec4 posQ = transform * vec4(vert.pos, 1.0);
	vert.pos = posQ.xyz / posQ.w;
	vert.normal = normalize(normTfm * vert.normal);
}

float Stack_Top(){
	return _Stack[_Stack_mTop];
}

void Stack_Push(bool val){
	_Stack_mTop++;
	_Stack[_Stack_mTop] = float(val);
}

float Stack_Pop(){
	float val = _Stack[_Stack_mTop];
	_Stack_mTop--;
	return val;
}

void Stack_Pop(out bool val){
	val = bool(_Stack[_Stack_mTop]);
	_Stack_mTop--;
}

void Stack_Push(float val){
	_Stack_mTop++;
	_Stack[_Stack_mTop] = val;
}

void Stack_Pop(out float val){
	val = _Stack[_Stack_mTop];
	_Stack_mTop--;
}

void Stack_Push(vec2 v2){
	_Stack_mTop += 2;
	_Stack[_Stack_mTop-1] = v2.x;
	_Stack[_Stack_mTop] = v2.y;
}

void Stack_Pop(out vec2 v2){
	v2.y = _Stack[_Stack_mTop];
	v2.x = _Stack[_Stack_mTop-1];
	_Stack_mTop -= 2;
}

void Stack_Push(vec3 v3){
	_Stack_mTop += 3;
	_Stack[_Stack_mTop-2] = v3.x;
	_Stack[_Stack_mTop-1] = v3.y;
	_Stack[_Stack_mTop] = v3.z;
}

void Stack_Pop(out vec3 v3){
	v3.z = _Stack[_Stack_mTop];
	v3.y = _Stack[_Stack_mTop-1];
	v3.x = _Stack[_Stack_mTop-2];
	_Stack_mTop -= 3;
}

void Stack_Push(struct Ray ray){
	Stack_Push(ray.origin);
	Stack_Push(ray.dir);
	Stack_Push(ray.color);
	Stack_Push(ray.tMax);
	Stack_Push(ray.time);
}

void Stack_Pop(out struct Ray ray){
	Stack_Pop(ray.time);
	Stack_Pop(ray.tMax);
	Stack_Pop(ray.color);
	Stack_Pop(ray.dir);
	Stack_Pop(ray.origin);
}

void Stack_Push(struct Vertex vert){
	Stack_Push(vert.pos);
	Stack_Push(vert.normal);
	Stack_Push(vert.uv);
}

void Stack_Pop(out struct Vertex vert){
	Stack_Pop(vert.uv);
	Stack_Pop(vert.normal);
	Stack_Pop(vert.pos);
}

void Stack_Push(struct HitRst hitRst){
	Stack_Push(hitRst.hit);
	Stack_Push(hitRst.vertex);
	Stack_Push(hitRst.matIdx);
	Stack_Push(hitRst.isMatCoverable);
}

void Stack_Pop(out struct HitRst hitRst){
	Stack_Pop(hitRst.isMatCoverable);
	Stack_Pop(hitRst.matIdx);
	Stack_Pop(hitRst.vertex);
	Stack_Pop(hitRst.hit);
}

int Stack_Size(){
	return _Stack_mTop + 1;
}

bool Stack_Empty(){
	return _Stack_mTop == -1;
}

void Stack_Clear(){
	_Stack_mTop = -1;
}

float RandXY(float x, float y){
    return fract(cos(dot(vec2(x,y), vec2(12.9898, 4.1414))) * 43758.5453);
}
 
float Rand(){
    float a = RandXY(TexCoords.x, rdSeed[0]);
    float b = RandXY(rdSeed[1], TexCoords.y);
    float c = RandXY(rdCnt++, rdSeed[2]);
    float d = RandXY(rdSeed[3], a);
    float e = RandXY(b, c);
    float f = RandXY(d, e);
 
    return f;
}
 
vec2 RandInSquare(){
    return vec2(Rand(), Rand());
}
 
vec2 RandInCircle(){
    vec2 rst;
    do {
        rst = vec2(Rand(), Rand())*2.0f - 1.0f;
    } while (dot(rst, rst) >= 1.0);
    return rst;
}
 
vec3 RandInSphere(){
    vec3 rst;
    do {
        rst = vec3(Rand(), Rand(), Rand())*2.0f - 1.0f;
    } while (dot(rst, rst) >= 1.0);
    return rst;
}

float At(sampler2D data, float idx){
    vec2 texCoords = vec2((idx+0.5)/textureSize(data, 0).x, 0.5);
    return texture2D(data, texCoords).x;
}

void GetPack(float idx, out vec4 pack){
	vec2 texCoords = vec2((idx+0.5)/textureSize(PackData, 0).x, 0.5);
	pack = texture2D(PackData, texCoords);
}

void GetPack(float idx, out vec3 pack){
	vec2 texCoords = vec2((idx+0.5)/textureSize(PackData, 0).x, 0.5);
	pack = texture2D(PackData, texCoords).xyz;
}

void LoadMat(float idx, out mat4 m){
	GetPack(idx  , m[0]);
	GetPack(idx+1, m[1]);
	GetPack(idx+2, m[2]);
	GetPack(idx+3, m[3]);
}

void LoadMat(float idx, out mat3 m){
	GetPack(idx  , m[0]);
	GetPack(idx+1, m[1]);
	GetPack(idx+2, m[2]);
}
 
float atan2(float y, float x){
    if(x>0){
        return atan(y/x);
    }
    else if(x<0){
        if(y>=0){
            return atan(y/x) + PI;
        }else{
            return atan(y/x) - PI;
        }
    }
    else{// x==0
        return sign(y) * PI / 2;
    }
}

vec2 Sphere2UV(vec3 normal) {
    vec2 uv;
    float phi = atan2(normal.z, normal.x);
    float theta = asin(normal.y);
    uv[0] = 1 - (phi + PI) / (2 * PI);
    uv[1] = (theta + PI/2) / PI;
    return uv;
}

float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi){
    float cosTheta = dot(viewDir, halfway);
    float R0 = pow((ratioNtNi - 1) / (ratioNtNi + 1), 2);
    //float R = R0 + (1 - R0) * pow(1 - cosTheta, 5);
	return mix(pow(1 - cosTheta, 5), 1, R0);
}

vec4 Intersect_RayTri(vec3 e, vec3 d, vec3 a, vec3 b, vec3 c){
	mat3 equation_A = mat3(vec3(a-b), vec3(a-c), d);

	//平行
	if (abs(determinant(equation_A)) < 0.00001)
		return vec4(0);

	vec3 equation_b = a - e;
	vec3 equation_X = inverse(equation_A) * equation_b;
	float alpha = 1 - equation_X[0] - equation_X[1];
	return vec4(alpha, equation_X);
}

bool Scatter_Material(inout struct Ray ray, struct Vertex vertex, float matIdx){
    float matType = At(MatData, matIdx);
    
    if(matType == MatT_Lambertian)
        return Scatter_Lambertian(ray, vertex, matIdx);
    else if(matType == MatT_Metal)
        return Scatter_Metal(ray, vertex, matIdx);
    else if(matType == MatT_Dielectric)
        return Scatter_Dielectric(ray, vertex, matIdx);
    else if(matType == MatT_Light)
		return Scatter_Light(ray, vertex, matIdx);
	else if(matType == MatT_Isotropic)
		return Scatter_Isotropic(ray, vertex, matIdx);
	else{
        ray.color = vec3(1,0,1);//以此提示材质存在问题
        return false;
    }
}

bool Scatter_Lambertian(inout struct Ray ray, struct Vertex vertex, float matIdx){
    float texIdx = At(MatData, matIdx+1);
    vec3 albedo = Value_Texture(vertex.uv, vertex.pos, texIdx);
    
    ray.dir = vertex.normal + RandInSphere();
    ray.origin = vertex.pos;
    ray.color *= albedo;
    ray.tMax = FLT_MAX;
    return true;
}

bool Scatter_Metal(inout struct Ray ray, struct Vertex vertex, float matIdx){
    float texIdx = At(MatData, matIdx+1);
    vec3 specular = Value_Texture(vertex.uv, vertex.pos, texIdx);
    float fuzz = At(MatData, matIdx+2);
    
    vec3 dir = reflect(ray.dir, vertex.normal);
    vec3 dirFuzz = dir + fuzz * RandInSphere();
    
    // 反射光线在表面之下
    if (dot(dirFuzz, vertex.normal) < 0) {
        ray.color = vec3(0);
        return false;
    }
    
    Ray_Update(ray, vertex.pos, dirFuzz, specular);
    return true;
}

bool Scatter_Dielectric(inout struct Ray ray, struct Vertex vertex, float matIdx){
    float refractIndex = At(MatData, matIdx+1);
    
    vec3 refractDir;
    vec3 reflectDir = reflect(ray.dir, vertex.normal);
    
    vec3 ud = normalize(ray.dir);
    vec3 un = normalize(vertex.normal);
    vec3 airViewDir;
    if (dot(ud,un) < 0) {//外部
        refractDir = refract(ud, un, 1.0f / refractIndex);
        airViewDir = -ud;
    }
    else {//内部
        refractDir = refract(ud, -un, refractIndex);
        if (refractDir == vec3(0)) {
            Ray_Update(ray, vertex.pos, reflectDir, vec3(1));
            return true;
        }
        
        airViewDir = refractDir;
    }
    
    float fresnelFactor = FresnelSchlick(airViewDir, un, refractIndex);
    //vec3 dir = Rand() > fresnelFactor ? refractDir : reflectDir;
	vec3 dir = mix(reflectDir, refractDir, step(fresnelFactor, Rand()));
    Ray_Update(ray, vertex.pos, dir, vec3(1));
    return true;
}

bool Scatter_Light(inout struct Ray ray, struct Vertex vertex, float matIdx){
	float texIdx = At(MatData, matIdx+1);
	float linear = At(MatData, matIdx+2);
	float quadratic = At(MatData, matIdx+3);

	float d = ray.tMax * length(ray.dir);
	float attDis = 1.0f / (1.0f + d * (linear + quadratic * d));
	float attAngle = abs(dot(normalize(ray.dir), vertex.normal));
	vec3 lightColor = Value_Texture(vertex.uv, vertex.pos, texIdx);
	ray.color *= attDis * attAngle * lightColor;
	//ray.color *= attDis * lightColor;

	return false;
}

bool Scatter_Isotropic(inout struct Ray ray, struct Vertex vertex, float matIdx){
	float texIdx = At(MatData, matIdx+1);

	vec3 attenuation = Value_Texture(vertex.uv, vertex.pos, texIdx);
	Ray_Update(ray, vertex.pos, RandInSphere(), attenuation);
	return true;
}

void RayIn_Scene(inout struct Ray ray, out struct HitRst finalHitRst){
    Stack_Push(3);//Group的 孩子指针 的位置
    finalHitRst.hit = false;
    while(!Stack_Empty()){
        float pIdx = Stack_Pop();
		float idx = At(SceneData, pIdx);
		if(idx <= 0){
			idx = -idx;
			float type = At(SceneData, idx);// 只可能是那些有子节点的类型
			if(type == HT_Group || type == HT_BVH_Node || type == HT_TriMesh ){
				float matIdx = At(SceneData, idx+1);
				if( matIdx == -1.0)
					continue;

				float in_tMax = Stack_Pop();// 进入节点时的tMax
				if (ray.tMax < in_tMax && finalHitRst.isMatCoverable == 1.0){
					finalHitRst.matIdx = matIdx;
					finalHitRst.isMatCoverable = At(SceneData, idx+2);
				}
			}else if(type == HT_Transform){
				float in_tMax = Stack_Pop();// 进入节点时的tMax
				mat4 tfmMat4;
				LoadMat(At(SceneData,idx+3), tfmMat4);
				Ray_Transform(ray, tfmMat4);
				if(ray.tMax < in_tMax){
					mat3 normTfmMat3;
					LoadMat(At(SceneData,idx+3), normTfmMat3);
					Vertex_Transform(finalHitRst.vertex, tfmMat4, normTfmMat3);
					if(finalHitRst.isMatCoverable == 1.0){
						float matIdx = At(SceneData, idx+1);
						if(matIdx != -1.0){
							finalHitRst.matIdx = matIdx;
							finalHitRst.isMatCoverable = At(SceneData, idx+2);
						}
					}
				}
			}
			else if(type == HT_Volume){
				float state = Stack_Pop();
				RayIn_Volume(idx, state, ray, finalHitRst);
			}
			//else
			//	;//do nothing

			continue;
		}
		
		Stack_Push(pIdx+1);

		float type = At(SceneData, idx);
		if(type == HT_Sphere)
			RayIn_Sphere(idx, ray, finalHitRst);
		else if(type == HT_Group){
			float matIdx = At(SceneData, idx+1);
			if( matIdx != -1.0)
				Stack_Push(ray.tMax);
			Stack_Push(idx+3);
		}
		else if(type == HT_BVH_Node || type == HT_TriMesh){
			if(AABB_Hit(ray, idx+3)){
				float matIdx = At(SceneData, idx+1);
				if( matIdx != -1.0)
					Stack_Push(ray.tMax);
				Stack_Push(idx+4);
			}
		}
		else if(type == HT_Triangle)
			RayIn_Triangle(idx, ray, finalHitRst);
		else if(type == HT_Transform){
			mat4 invTfmMat4;
			LoadMat(At(SceneData,idx+3)+4, invTfmMat4);
			Ray_Transform(ray, invTfmMat4);
			Stack_Push(ray.tMax);
			Stack_Push(idx+4);
		}
		else if(type == HT_Volume)
			RayIn_Volume(idx, 0.0, ray, finalHitRst);
		//else// not supported type
		    //;
    }
}

void RayIn_Sphere(float idx, inout struct Ray ray, inout struct HitRst hitRst){
	vec4 center_radius;
	GetPack(At(SceneData, idx+3), center_radius);
    vec3 center = center_radius.xyz;
    float radius = center_radius[3];
    
    vec3 oc = ray.origin - center;
    float a = dot(ray.dir, ray.dir);
    float b = dot(oc, ray.dir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    
    if (discriminant <= 0 || a == 0 || radius == 0)
        return;
    
    float t = (-b - sqrt(discriminant)) / a;
    if (t > ray.tMax || t < tMin) {
        t = (-b + sqrt(discriminant)) / a;
        if (t > ray.tMax || t < tMin)
            return;
    }
    
    ray.tMax = t;
    
    hitRst.hit = true;
    hitRst.vertex.pos = ray.origin + t * ray.dir;
    hitRst.vertex.normal = (hitRst.vertex.pos - center) / radius;
    hitRst.vertex.uv = Sphere2UV(hitRst.vertex.normal);
    float matIdx = At(SceneData, idx+1);
	float isMatCoverable = At(SceneData, idx+2);
    hitRst.matIdx = matIdx;
    hitRst.isMatCoverable = isMatCoverable;
}

void RayIn_Triangle(float idx, inout struct Ray ray, inout struct HitRst hitRst){
	struct Vertex A, B, C;
	float vertexABC_pack4_idx = At(SceneData, idx+3);
	Vertex_Load(vertexABC_pack4_idx  , A);
	Vertex_Load(vertexABC_pack4_idx+2, B);
	Vertex_Load(vertexABC_pack4_idx+4, C);

	vec4 abgt = Intersect_RayTri(ray.origin, ray.dir, A.pos, B.pos, C.pos);
	if (abgt == vec4(0) ||
		any(lessThan(abgt,vec4(0,0,0,tMin))) ||
		any(greaterThan(abgt,vec4(1,1,1,ray.tMax)))
		//|| abgt[0] < 0 || abgt[0] > 1
		//|| abgt[1] < 0 || abgt[1] > 1
		//|| abgt[2] < 0 || abgt[2] > 1
		//|| abgt[3] < tMin || abgt[3] > ray.tMax
		)
		return;

	hitRst.hit = true;
	Vertex_Interpolate(abgt.xyz, A, B, C, hitRst.vertex);
	float matIdx = At(SceneData, idx+1);
	float isMatCoverable = At(SceneData, idx+2);
	hitRst.matIdx = matIdx;
	hitRst.isMatCoverable = isMatCoverable;
	ray.tMax = abgt[3];
}

void RayIn_Volume(float idx, float state, inout struct Ray ray, inout struct HitRst hitRst){
	if(state==0.0){
		float boundaryIdx = At(SceneData, idx+4);
		if(boundaryIdx == -1.0)
			return;

		Stack_Push(ray);// before hitRst
		Stack_Push(hitRst);
		Stack_Push(ray.tMax);// original ray's tMax
		Stack_Push(1.0);//state
		Stack_Push(idx+4);// ptr to ptr to boundary
	}else if(state==1.0){
		float in_tMax = Stack_Pop();// original ray's tMax
		if(ray.tMax >= in_tMax){// hit boundary ray
			Stack_Pop(hitRst);
			Stack_Pop(ray);// after hitRst
			return;
		}
		
		Stack_Push(ray.tMax);// hit boundary ray's tMax

		// reverse ray
		Ray_Update(ray, ray.origin + tMin*1.5*ray.dir, -ray.dir, vec3(0));
		hitRst = HitRst_InValid;
		
		Stack_Push(2.0);// state
		Stack_Push(idx+4);// ptr to ptr to boundary
	}else{
		float t0;
		float tMaxFromT0;
		if(state==2.0){
			if(!hitRst.hit){// reverse ray not hit, so ray's origin is in the volume
				float t0 = Stack_Top();// hit boundary ray's tMax, don't pop it, will use it later(in state 3.0)

				// ont boundary ray
				vec3 origin = ray.origin + tMin*1.5*ray.dir - ray.dir * t0;
				Ray_Update(ray, origin, -ray.dir, vec3(0));
				hitRst = HitRst_InValid;

				Stack_Push(3.0);// state
				Stack_Push(idx+4);// ptr to ptr to boundary
				return;
			}

			t0 = 0;
			tMaxFromT0 = Stack_Pop();// hit boundary ray's tMax
		}else if(state == 3.0){
			t0 = Stack_Pop();// hit boundary ray's tMax

			if(!hitRst.hit){// on boundary Ray
				Stack_Pop(hitRst);
				Stack_Pop(ray);
				return;
			}

			tMaxFromT0 = ray.tMax;// on boundary Ray's tMax
		}
		else //error state, it's a logic error
			return;
			
		Stack_Pop(hitRst);
		Stack_Pop(ray);

		float t1 = min(ray.tMax, t0 + tMaxFromT0);
		//此处的 len 未考虑 transform 的 scale
		float lenInVolume = (t1 - t0) * length(ray.dir);

		float density = At(SceneData, idx+3);
		// p = C * dL
		// p(L) = lim(n->inf, (1 - CL/n)^n) = exp(-CL)
		// L = -(1/C)ln(pL)
		float hitLen = -(1.0f / density)*log(Rand());

		if (hitLen >= lenInVolume)
			return;

		float tFinal = t0 + hitLen / length(ray.dir);
		ray.tMax = tFinal;

		hitRst.hit = true;
		hitRst.vertex.pos = ray.origin + tFinal * ray.dir;
		hitRst.matIdx = At(SceneData, idx+1);
		hitRst.isMatCoverable = At(SceneData, idx+2);
	}
}

bool AABB_Hit(struct Ray ray, float idx){
	float AABB_pack4_idx = At(SceneData, idx);
	vec3 minP, maxP;
	GetPack(AABB_pack4_idx  , minP);
	GetPack(AABB_pack4_idx+1, maxP);

	vec3 origin = ray.origin;
	vec3 dir = ray.dir;
	float local_tMin = tMin;
	float tMax = ray.tMax;
	for (int i = 0; i < 3; i++) {
		float invD = 1.0f / dir[i];
		float t0 = (minP[i] - origin[i]) * invD;
		float t1 = (maxP[i] - origin[i]) * invD;
		if (invD < 0.0f){
			float tmp = t0;
			t0 = t1;
			t1 = tmp;
		}

		local_tMin = max(t0, local_tMin);
		tMax = min(t1, tMax);
		if (tMax <= local_tMin)
			return false;
	}
	return true;
}
 
vec3 Value_Texture(vec2 uv, vec3 p, float texIdx){
	float type = At(TexData, texIdx);
	if(type == TexT_ConstTexture)
		return Value_ConstTexture(texIdx);
	else if(type == TexT_ImgTexture)
		return Value_ImgTexture(uv, texIdx);
	else
		return vec3(1,0,1);
}
 
vec3 Value_ConstTexture(float texIdx){
	float color_pack4_idx = At(TexData, texIdx+1);
	vec3 color;
	GetPack(color_pack4_idx, color);
	return color;
}

vec3 Value_ImgTexture(vec2 uv, float texIdx){
	int texArrIdx = int(At(TexData, texIdx+1));
	return texture2D(TexArr[texArrIdx], uv).xyz;
}

void Ray_Update(inout struct Ray ray, vec3 origin, vec3 dir, vec3 attenuation){
    ray.origin = origin;
    ray.dir = dir;
    ray.color *= attenuation;
    ray.tMax = FLT_MAX;
}

void Ray_Transform(inout struct Ray ray, mat4 transform){
	ray.dir = mat3(transform) * ray.dir;
	vec4 originQ = transform * vec4(ray.origin, 1.0f);
	ray.origin = originQ.xyz / originQ.w;
}

void Camera_GenRay(out struct Ray ray){
    vec2 st = TexCoords + RandInSquare() / textureSize(origin_curRayNum, 0);
    vec2 rd = camera.lenR * RandInCircle();
    
    ray.origin = camera.pos + rd.x * camera.right + rd.y * camera.up;
    ray.dir = camera.BL_Corner + st.s * camera.horizontal + st.t * camera.vertical - ray.origin;
    ray.color = vec3(1);
    ray.tMax = FLT_MAX;
    ray.time = mix(camera.t0, camera.t1, Rand());
}

bool GetRay(out struct Ray ray){
    vec4 val_origin_curRayNum = texture(origin_curRayNum, TexCoords);
	curRayNum = val_origin_curRayNum.w;
    if(curRayNum >= RayNumMax)
        return false;

    vec4 val_dir_tMax = texture(dir_tMax, TexCoords);
    if(val_dir_tMax.w == 0){
        Camera_GenRay(ray);
    }else{
        ray.origin = val_origin_curRayNum.xyz;
        ray.dir = val_dir_tMax.xyz;
        ray.tMax = val_dir_tMax.w;
        vec4 val_color_time = texture(color_time, TexCoords);
        ray.color = val_color_time.xyz;
        ray.time = val_color_time.w;
    }
	return true;
}

void WriteRay(struct Ray ray, int mode){
    vec3 color = texture(rayTracingRst, TexCoords).xyz;

    if(mode == 0){//最终没有击中光源
        ray.tMax = 0;
		#if ENABLE_GAMMA_CORRECTION
			color = color * sqrt( curRayNum / (curRayNum + 1));
		#else
			color = color * curRayNum / (curRayNum + 1);
		#endif
        curRayNum = min(curRayNum + 1, RayNumMax);
    }
    else if(mode == 1){//击中光源
        ray.tMax = 0;
		#if ENABLE_GAMMA_CORRECTION
			color = sqrt((color * color * curRayNum + ray.color) / (curRayNum + 1));
		#else
			color = (color * curRayNum + ray.color) / (curRayNum + 1);
		#endif
        curRayNum = min(curRayNum + 1, RayNumMax);
    }
    //else if(mode == 2){//继续追踪
    //  //do nothing
    //}
    else if(mode == 3){
        curRayNum = RayNumMax + 100;
    }else
        ;//do nothing

    out_origin_curRayNum = vec4(ray.origin, curRayNum);
    out_dir_tMax = vec4(ray.dir, ray.tMax);
    out_color_time = vec4(ray.color, ray.time);
    out_rayTracingRst = color;
}

void RayTracer(){
	struct Ray ray;
    if(!GetRay(ray)){
        WriteRay(ray, 3);
        return;
    }
    
    if(Rand() > rayP){
        WriteRay(ray, 0);//最终没有击中光源
        return;
    }
    
    struct HitRst finalHitRst;
	RayIn_Scene(ray, finalHitRst);

    if(finalHitRst.hit){
        bool rayOut = Scatter_Material(ray, finalHitRst.vertex, finalHitRst.matIdx);
        
        if(rayOut)
            WriteRay(ray, 2);//继续追踪
        else
            WriteRay(ray, 1);//碰到光源
    }else{// default sky
        float t = 0.5f * (normalize(ray.dir).y + 1.0f);
        vec3 white = vec3(1.0f, 1.0f, 1.0f);
        vec3 blue = vec3(0.5f, 0.7f, 1.0f);
        vec3 lightColor = (1 - t) * white + t * blue;
        ray.color *= lightColor;
        WriteRay(ray, 1);
    }
}


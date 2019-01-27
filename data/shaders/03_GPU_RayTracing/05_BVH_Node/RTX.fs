#version 330 core

layout (location = 0) out vec4 out_origin_curRayNum;
layout (location = 1) out vec4 out_dir_tMax;
layout (location = 2) out vec4 out_color_time;
layout (location = 3) out vec3 out_rayTracingRst;

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
void Camera_GenRay();

struct Ray{
    vec3 origin;
    vec3 dir;
    highp vec3 color;
    float tMax;
    float time;
    highp float curRayNum;//这里使用float以统一
};
void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation);

struct Vertex{
    vec3 pos;
    vec3 normal;
    vec2 uv;
};
struct Vertex Vertex_InValid = struct Vertex(vec3(0), vec3(0), vec2(0));

struct HitRst{
    bool hit;
    struct Vertex vertex;
    float matIdx;
};
struct HitRst HitRst_InValid = struct HitRst(false, Vertex_InValid, -1);

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;//若tMax为0, 则表示该光线无效
uniform sampler2D color_time;
uniform sampler2D RTXRst;

uniform sampler2D SceneData;
uniform sampler2D MatData;
uniform sampler2D TexData;

uniform struct Camera camera;
uniform float rdSeed[4];
uniform float RayNumMax;
uniform bool enableGammaCorrection;

const float PI = 3.1415926;
const float tMin = 0.001;
const float FLT_MAX = 99999999999999999999999999999999999999.0;
const float rayP = 50.0/51.0;// depth = p/(1-p) --> p = depth/(depth+1)
const float HT_Sphere         = 0.0;
const float HT_Group          = 1.0;
const float HT_BVH_Node       = 2.0;
const float MatT_Lambertian   = 0.0;
const float MatT_Metal        = 1.0;
const float MatT_Dielectric   = 2.0;
const float TexT_ConstTexture = 0.0;

int rdCnt = 0;
in vec2 TexCoords;
struct Ray gRay;

float _Stack[100];
int _Stack_mTop = -1;
bool Stack_Empty();
float Stack_Top();
void Stack_Push(float val);
float Stack_Pop();
void Stack_Acc();

float RandXY(float x, float y);// [0.0, 1.0)
float Rand();// [0.0, 1.0)
vec2 RandInSquare();
vec2 RandInCircle();
vec3 RandInSphere();
float At(sampler2D data, float idx);
float atan2(float y, float x);
vec2 Sphere2UV(vec3 normal);
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);
void SetRay();
void WriteRay(int mode);
struct HitRst RayIn_Scene();
struct HitRst RayIn_Sphere(float idx);
bool AABB_Hit(float idx);
bool Scatter_Material(struct Vertex vertex, float matIdx);
bool Scatter_Lambertian(struct Vertex vertex, float matIdx);
bool Scatter_Metal(struct Vertex vertex, float matIdx);
bool Scatter_Dielectric(struct Vertex vertex, float matIdx);
vec3 Value_Texture(vec2 uv, vec3 p, float texIdx);
vec3 Value_ConstTexture(float texIdx);
void RayTracer();

void main(){
	//if(TexCoords.y<0.33)
	//	out_rayTracingRst=vec3(At(SceneData, TexCoords.x*25),0,0);
	//else if(TexCoords.y<0.67)
	//	out_rayTracingRst=vec3(At(SceneData, TexCoords.x*4),0,0);
	//else
	//	out_rayTracingRst=vec3(At(SceneData, TexCoords.x*8),0,0);
	RayTracer();
}

float Stack_Top(){
	//if(_Stack_mTop == -1)
	//	return -1.0;

	return _Stack[_Stack_mTop];
}

void Stack_Push(float val){
	_Stack_mTop++;
	_Stack[_Stack_mTop] = val;
}

float Stack_Pop(){
	float val = _Stack[_Stack_mTop];
	_Stack_mTop--;
	return val;
}

int Stack_Size(){
	return _Stack_mTop + 1;
}

bool Stack_Empty(){
	return _Stack_mTop == -1;
}

void Stack_Acc(){
	//if(_Stack_mTop == -1)
	//	return false;

	_Stack[_Stack_mTop] += 1.0;
	//return true;
}

float RandXY(float x, float y){
    return fract(cos(x * (12.9898) + y * (4.1414)) * 43758.5453);
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
    float R = R0 + (1 - R0) * pow(1 - cosTheta, 5);
    return R;
}

bool Scatter_Material(struct Vertex vertex, float matIdx){
    float matType = At(MatData, matIdx);
    
    if(matType == MatT_Lambertian)
        return Scatter_Lambertian(vertex, matIdx);
    else if(matType == MatT_Metal)
        return Scatter_Metal(vertex, matIdx);
    else if(matType == MatT_Dielectric)
        return Scatter_Dielectric(vertex, matIdx);
    else{
        gRay.color = vec3(1,0,1);//以此提示材质存在问题
        return false;
    }
}

bool Scatter_Lambertian(struct Vertex vertex, float matIdx){
    float texIdx = At(MatData, matIdx+1);
    vec3 albedo = Value_Texture(vertex.uv, vertex.pos, texIdx);
    
    gRay.dir = vertex.normal + RandInSphere();
    gRay.origin = vertex.pos;
    gRay.color *= albedo;
    gRay.tMax = FLT_MAX;
    return true;
}

bool Scatter_Metal(struct Vertex vertex, float matIdx){
    float texIdx = At(MatData, matIdx+1);
    vec3 specular = Value_Texture(vertex.uv, vertex.pos, texIdx);
    float fuzz = At(MatData, matIdx+2);
    
    vec3 dir = reflect(gRay.dir, vertex.normal);
    vec3 dirFuzz = dir + fuzz * RandInSphere();
    
    // 反射光线在表面之下
    if (dot(dirFuzz, vertex.normal) < 0) {
        gRay.color = vec3(0);
        return false;
    }
    
    Ray_Update(vertex.pos, dirFuzz, specular);
    return true;
}

bool Scatter_Dielectric(struct Vertex vertex, float matIdx){
    float refractIndex = At(MatData, matIdx+1);
    
    vec3 refractDir;
    vec3 reflectDir = reflect(gRay.dir, vertex.normal);
    
    vec3 ud = normalize(gRay.dir);
    vec3 un = normalize(vertex.normal);
    vec3 airViewDir;
    if (dot(ud,un) < 0) {//外部
        refractDir = refract(ud, un, 1.0f / refractIndex);
        airViewDir = -ud;
    }
    else {//内部
        refractDir = refract(ud, -un, refractIndex);
        if (refractDir == vec3(0)) {
            Ray_Update(vertex.pos, reflectDir, vec3(1));
            return true;
        }
        
        airViewDir = refractDir;
    }
    
    float fresnelFactor = FresnelSchlick(airViewDir, un, refractIndex);
    vec3 dir = Rand() > fresnelFactor ? refractDir : reflectDir;
    Ray_Update(vertex.pos, dir, vec3(1));
    return true;
}

struct HitRst RayIn_Scene(){
    Stack_Push(9);//Group的 孩子指针 的位置
    struct HitRst finalHitRst = HitRst_InValid;
    while(!Stack_Empty()){
        float pIdx = Stack_Top();
		float idx = At(SceneData, pIdx);
		if(idx == -1.0){
			Stack_Pop();
			if(Stack_Empty())
				return finalHitRst;

			//由于暂时没有离开节点时需要做的事情, 因此先都注释掉
			//float pIdx = Stack_Top();
			//float idx = At(SceneData, pIdx);// idx != -1
			//float type = At(SceneData, idx);// 只可能是那些有子节点的类型
			//if(type == HT_Group)
			//	;// 修改材质指针

			Stack_Acc();
			continue;
		}
		
		float type = At(SceneData, idx);
		if(type == HT_Sphere){
			struct HitRst hitRst = RayIn_Sphere(idx);
			if(hitRst.hit)
				finalHitRst = hitRst;

			Stack_Acc();
		}
		else if(type == HT_Group)
			Stack_Push(idx+9);
		else if(type == HT_BVH_Node){
			if(AABB_Hit(idx+3))
				Stack_Push(idx+9);
			else
				Stack_Acc();
		}
		else
		    Stack_Acc();
    }
    
    return finalHitRst;
}

struct HitRst RayIn_Sphere(float idx){
    float matIdx = At(SceneData, idx+1);
    vec3 center = vec3(At(SceneData, idx+9), At(SceneData, idx+10), At(SceneData, idx+11));
    float radius = At(SceneData, idx+12);
    
    struct HitRst hitRst;
    hitRst.hit = false;
    
    vec3 oc = gRay.origin - center;
    float a = dot(gRay.dir, gRay.dir);
    float b = dot(oc, gRay.dir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    
    if (discriminant <= 0 || a == 0 || radius == 0)
        return hitRst;
    
    float t = (-b - sqrt(discriminant)) / a;
    if (t > gRay.tMax || t < tMin) {
        t = (-b + sqrt(discriminant)) / a;
        if (t > gRay.tMax || t < tMin)
            return hitRst;
    }
    
    gRay.tMax = t;
    
    hitRst.hit = true;
    hitRst.vertex.pos = gRay.origin + t * gRay.dir;
    hitRst.vertex.normal = (hitRst.vertex.pos - center) / radius;
    hitRst.vertex.uv = Sphere2UV(hitRst.vertex.normal);
    hitRst.matIdx = matIdx;
    
    return hitRst;
}

bool AABB_Hit(float idx){
	vec3 minP = vec3(At(SceneData, idx), At(SceneData, idx+1), At(SceneData, idx+2));
	vec3 maxP = vec3(At(SceneData, idx+3), At(SceneData, idx+4), At(SceneData, idx+5));

	vec3 origin = gRay.origin;
	vec3 dir = gRay.dir;
	float local_tMin = tMin;
	float tMax = gRay.tMax;
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
	if(type == TexT_ConstTexture){
		return Value_ConstTexture(texIdx);
	}else
		return vec3(1,0,1);
}
 
vec3 Value_ConstTexture(float texIdx){
	vec3 color = vec3(At(TexData, texIdx+1), At(TexData, texIdx+2), At(TexData, texIdx+3));
	return color;
}

void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation){
    gRay.origin = origin;
    gRay.dir = dir;
    gRay.color *= attenuation;
    gRay.tMax = FLT_MAX;
}

void Camera_GenRay(){
    vec2 st = TexCoords + RandInSquare() / textureSize(origin_curRayNum, 0);
    vec2 rd = camera.lenR * RandInCircle();
    
    gRay.origin = camera.pos + rd.x * camera.right + rd.y * camera.up;
    gRay.dir = camera.BL_Corner + st.s * camera.horizontal + st.t * camera.vertical - gRay.origin;
    gRay.color = vec3(1);
    gRay.tMax = FLT_MAX;
    gRay.time = mix(camera.t0, camera.t1, Rand());
}

void SetRay(){
    vec4 val_origin_curRayNum = texture(origin_curRayNum, TexCoords);
    gRay.curRayNum = val_origin_curRayNum.w;
    if(gRay.curRayNum >= RayNumMax)
        return;
    vec4 val_dir_tMax = texture(dir_tMax, TexCoords);
    if(val_dir_tMax.w == 0){
        Camera_GenRay();
    }else{
        gRay.origin = val_origin_curRayNum.xyz;
        gRay.dir = val_dir_tMax.xyz;
        gRay.tMax = val_dir_tMax.w;
        vec4 val_color_time = texture(color_time, TexCoords);
        gRay.color = val_color_time.xyz;
        gRay.time = val_color_time.w;
    }
}

void WriteRay(int mode){
    vec3 color = texture(rayTracingRst, TexCoords).xyz;

    if(mode == 0){//最终没有击中光源
        gRay.tMax = 0;
        color *= gRay.curRayNum / (gRay.curRayNum + 1);
        gRay.curRayNum = min(gRay.curRayNum + 1, RayNumMax);
    }
    else if(mode == 1){//击中光源
        gRay.tMax = 0;
		color = (color * gRay.curRayNum + (enableGammaCorrection ? sqrt(gRay.color) : gRay.color)) / (gRay.curRayNum + 1);
        gRay.curRayNum = min(gRay.curRayNum + 1, RayNumMax);
    }
    //else if(mode == 2){//继续追踪
    //  //do nothing
    //}
    else if(mode == 3){
        gRay.curRayNum = RayNumMax + 100;
    }else
        ;//do nothing

    out_origin_curRayNum = vec4(gRay.origin, gRay.curRayNum);
    out_dir_tMax = vec4(gRay.dir, gRay.tMax);
    out_color_time = vec4(gRay.color, gRay.time);
    out_rayTracingRst = color;
}

void RayTracer(){
    SetRay();
    if(gRay.curRayNum >= RayNumMax){
        WriteRay(3);
        return;
    }
    
    if(Rand() > rayP){
        WriteRay(0);//最终没有击中光源
        return;
    }
    
    struct HitRst finalHitRst;
	finalHitRst = RayIn_Scene();

    if(finalHitRst.hit){
        bool rayOut = Scatter_Material(finalHitRst.vertex, finalHitRst.matIdx);
        
        if(rayOut)
            WriteRay(2);//继续追踪
        else
            WriteRay(1);
    }else{// sky
        float t = 0.5f * (normalize(gRay.dir).y + 1.0f);
        vec3 white = vec3(1.0f, 1.0f, 1.0f);
        vec3 blue = vec3(0.5f, 0.7f, 1.0f);
        vec3 lightColor = (1 - t) * white + t * blue;
        gRay.color *= lightColor;
        WriteRay(1);
    }
}


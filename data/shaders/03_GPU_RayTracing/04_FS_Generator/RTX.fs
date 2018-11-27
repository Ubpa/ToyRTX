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
    int matIdx;
};
struct HitRst HitRst_InValid = struct HitRst(false, Vertex_InValid, -1);

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;//若tMax为0, 则表示该光线无效
uniform sampler2D color_time;
uniform sampler2D rayTracingRst;
uniform struct Camera camera;
uniform float rdSeed[4];
uniform float RayNumMax;

const float PI = 3.1415926;
const float tMin = 0.001;
const float FLT_MAX = 99999999999999999999999999999999999999.0;
const float rayP = 50.0/51.0;// depth = p/(1-p) --> p = depth/(depth+1)
const int HT_Sphere         = 0;
const int HT_Group          = 1;
const int MatT_Lambertian   = 0;
const int MatT_Metal        = 1;
const int MatT_Dielectric   = 2;
const int TexT_ConstTexture = 0;

int rdCnt = 0;
in vec2 TexCoords;
struct Ray gRay;

const float SceneData[44] = float[](
    1, -1, 1, 5, 9, 16, 23, 30, 37, 0, 5, 0, 0, -100.5, -1, 
    100, 0, 0, 0, 0, 0, -1, 0.5, 0, 9, 0, -1, 0, -1, 0.5, 0, 
    7, 0, -1, 0, -1, -0.45, 0, 2, 0, 1, 0, -1, 0.5
);

const float MatData[11] = float[](
0, 8, 1, 4, 0, 0, 0, 2, 1.5, 2, 1.5
);

const float TexData[12] = float[](
0, 0.5, 0.5, 0.5, 0, 0.1, 0.2, 0.5, 0, 0.8, 0.8, 0
);

float RandXY(float x, float y);// [0.0, 1.0)
float Rand();// [0.0, 1.0)
vec2 RandInSquare();
vec2 RandInCircle();
vec3 RandInSphere();
float atan2(float y, float x);
vec2 Sphere2UV(vec3 normal);
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);
void SetRay();
void WriteRay(int mode);
struct HitRst RayIn_Scene();
struct HitRst RayIn_Sphere(int idx);
bool Scatter_Material(struct Vertex vertex, int matIdx);
bool Scatter_Lambertian(struct Vertex vertex, int matIdx);
bool Scatter_Metal(struct Vertex vertex, int matIdx);
bool Scatter_Dielectric(struct Vertex vertex, int matIdx);
vec3 Value_Texture(vec2 uv, vec3 p, int texIdx);
vec3 Value_ConstTexture(int texIdx);
void RayTracer();

void main(){
    RayTracer();
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
    float R = R0 + (1 - R0)*pow(1 - cosTheta, 5);
    return R;
}

bool Scatter_Material(struct Vertex vertex, int matIdx){
    int matType = int(MatData[matIdx]);
    
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

bool Scatter_Lambertian(struct Vertex vertex, int matIdx){
    int texIdx = int(MatData[matIdx+1]);
    vec3 albedo = Value_Texture(vertex.uv, vertex.pos, texIdx);
    
    gRay.dir = vertex.normal + RandInSphere();
    gRay.origin = vertex.pos;
    gRay.color *= albedo;
    gRay.tMax = FLT_MAX;
    return true;
}

bool Scatter_Metal(struct Vertex vertex, int matIdx){
    int texIdx = int(MatData[matIdx+1]);
    vec3 specular = Value_Texture(vertex.uv, vertex.pos, texIdx);
    float fuzz = MatData[matIdx+4];
    
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

bool Scatter_Dielectric(struct Vertex vertex, int matIdx){
    float refractIndex = MatData[matIdx+1];
    
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
    int idxStack[100];
    int idxStackSize = 0;
    struct HitRst finalHitRst = HitRst_InValid;
    
    idxStack[idxStackSize++] = 0;
    while(idxStackSize > 0){
        int idx = idxStack[--idxStackSize];
        
        int type = int(SceneData[idx]);//32b float 在 1677w 时出现误差, 故可接受
        if(type == HT_Sphere){
            struct HitRst hitRst = RayIn_Sphere(idx);
            if(hitRst.hit)
                finalHitRst = hitRst;
        }
        else if(type == HT_Group){
            int childrenNum = int(SceneData[idx+3]);
            for(int i=0; i < childrenNum; i++)
                idxStack[idxStackSize++] = int(SceneData[idx+4+i]);
        }
        //else
        //    ;// do nothing
    }
    
    return finalHitRst;
}

struct HitRst RayIn_Sphere(int idx){
    int matIdx = int(SceneData[idx+1]);//32b float 在 1677w 时出现误差, 故可接受
    vec3 center = vec3(SceneData[idx+3], SceneData[idx+4], SceneData[idx+5]);
    float radius = SceneData[idx+6];
    
    struct HitRst hitRst;
    hitRst.hit = false;
    
    vec3 oc = gRay.origin - center;
    float a = dot(gRay.dir, gRay.dir);
    float b = dot(oc, gRay.dir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    
    if (discriminant <= 0)
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
 
vec3 Value_Texture(vec2 uv, vec3 p, int texIdx){
	int type = int(TexData[texIdx]);
	if(type == TexT_ConstTexture){
		return Value_ConstTexture(texIdx);
	}else
		return vec3(1,0,1);
}
 
vec3 Value_ConstTexture(int texIdx){
	vec3 color = vec3(TexData[texIdx+1], TexData[texIdx+2], TexData[texIdx+3]);
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
        color = (color * gRay.curRayNum + gRay.color) / (gRay.curRayNum + 1);
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
    
    struct HitRst finalHitRst = RayIn_Scene();
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


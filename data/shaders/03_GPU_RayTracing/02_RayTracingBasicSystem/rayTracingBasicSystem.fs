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

struct Ray{
    vec3 origin;
    vec3 dir;
    highp vec3 color;
    float tMax;
	float time;
    highp float curRayNum;//这里使用float以统一
};

struct Vertex{
	vec3 pos;
	vec3 normal;
	vec2 uv;
};

struct HitRst{
	bool hit;
	struct Vertex vertex;
	int matIdx;
};

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;//若tMax为0, 则表示该光线无效
uniform sampler2D color_time;
uniform sampler2D RTXRst;
uniform struct Camera camera;
uniform float rdSeed[4];
uniform float RayNumMax;

const float PI = 3.1415926;
const float tMin = 0.001;
const float FLTMAX = 99999999999999999999999999999999999999.0;
const float rayP = 50.0/51.0;// depth = p/(1-p) --> p = depth/(depth+1)
const int sphereNum = 2;
const float Scene[12] = float[](
	//type	//matIdx	//center		//radius
	0,		0,			0,		0, -1,	0.5,
	0,		0,			0, -100.5, -1,	100
);
const float Material[4] = float[](
	//type	//color
	0,		0.5, 0.5, 0.5
);

int rdCnt = 0;
in vec2 TexCoords;
struct Ray gRay;

float RandXY(float x, float y);// [0.0, 1.0)
float Rand(vec2 TexCoords, float frameTime, int rdCnt);// [0.0, 1.0)
vec2 RandInSquare();
vec2 RandInCircle();
vec3 RandInSphere();
float atan2(float y, float x);
vec2 Sphere2UV(vec3 normal);
void GenRay();
void SetRay();
void WriteRay(int mode);
struct HitRst RayIn_Sphere(int idx);
bool Scatter_Lambertian(struct Vertex vertex, int matIdx);
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

void GenRay(){
	vec2 st = TexCoords + RandInSquare() / textureSize(origin_curRayNum, 0);
	vec2 rd = camera.lenR * RandInCircle();

	gRay.origin = camera.pos + rd.x * camera.right + rd.y * camera.up;
	gRay.dir = camera.BL_Corner + st.s * camera.horizontal + st.t * camera.vertical - gRay.origin;
	gRay.color = vec3(1);
	gRay.tMax = FLTMAX;
	gRay.time = mix(camera.t0, camera.t1, Rand());
}

void SetRay(){
	vec4 val_origin_curRayNum = texture(origin_curRayNum, TexCoords);
	gRay.curRayNum = val_origin_curRayNum.w;
	if(gRay.curRayNum >= RayNumMax)
		return;
	vec4 val_dir_tMax = texture(dir_tMax, TexCoords);
	if(val_dir_tMax.w == 0){
		GenRay();
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
	//	//do nothing
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

bool Scatter_Lambertian(struct Vertex vertex, int matIdx){
	vec3 albedo = vec3(Material[matIdx+1], Material[matIdx+2], Material[matIdx+3]);

	gRay.dir = vertex.normal + RandInSphere();
	gRay.origin = vertex.pos;
	gRay.color *= albedo;
	gRay.tMax = FLTMAX;
	return true;
}

struct HitRst RayIn_Sphere(int idx){
	int matIdx = int(Scene[idx+1]);//32b float 在 1677w 时出现误差, 故可接受
    vec3 center = vec3(Scene[idx+2], Scene[idx+3], Scene[idx+4]);
    float radius = Scene[idx+5];

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
	finalHitRst.hit = false;
	int curIdx = 0;
	for(int i=0; i<sphereNum; i++){
		int hitableType = int(Scene[curIdx]);//32b float 在 1677w 时出现误差, 故可接受
		struct HitRst hitRst;
		hitRst.hit = false;
		if(hitableType == 0){//球
			hitRst = RayIn_Sphere(curIdx);
			curIdx += 6;
		}

		if(hitRst.hit){
			finalHitRst = hitRst;
		}
	}

	if(finalHitRst.hit){
		if(finalHitRst.matIdx == -1){
			gRay.color = vec3(1,0,1);//以此提示材质存在问题
			WriteRay(1);//击中光源
			return;
		}

		float matType = Material[finalHitRst.matIdx];
		if(matType == 0){// Lambertian
			bool rayOut = Scatter_Lambertian(finalHitRst.vertex, finalHitRst.matIdx);
			if(rayOut)
				WriteRay(2);//继续追踪
			else
				WriteRay(1);
		}
	}else{// sky
		float t = 0.5f * (normalize(gRay.dir).y + 1.0f);
		vec3 white = vec3(1.0f, 1.0f, 1.0f);
		vec3 blue = vec3(0.5f, 0.7f, 1.0f);
		vec3 lightColor = (1 - t) * white + t * blue;
		gRay.color *= lightColor;
		WriteRay(1);
	}
}


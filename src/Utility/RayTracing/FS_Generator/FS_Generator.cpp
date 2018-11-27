#include <RayTracing/FS_Generator.h>

#include "GenFS_HV.h"
#include "GenFS_MV.h"
#include "GenFS_TV.h"

#include <RayTracing/Hitable.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace std;

FS_Generator::FS_Generator(const CppUtility::Other::CPtr<Hitable> & scene)
	: hitableVisitor(ToPtr(new GenFS_HV)),
	matVisitor(ToPtr(new GenFS_MV)),
	texVisitor(ToPtr(new GenFS_TV))
{
	if (scene == NULL)
		return;

	scene->Accept(hitableVisitor);
	hitableVisitor->Accept(matVisitor);
	matVisitor->Accept(texVisitor);
}


const vector<float> FS_Generator::GetSceneData() const {
	return hitableVisitor->GetSceneData();
}

const vector<float> FS_Generator::GetMatData() const {
	return matVisitor->GetMatData();
}

const vector<float> FS_Generator::GetTexData() const {
	return texVisitor->GetTexData();
}

const string FS_Generator::BuildFS() {
	stringstream shaderSS;

	Version(shaderSS);
	LayOut(shaderSS);
	Struct(shaderSS);
	Uniform(shaderSS);
	Variable(shaderSS);
	FuncDefine(shaderSS);

	Main(shaderSS);

	RandFunc(shaderSS);
	MathFunc(shaderSS);
	Scatter(shaderSS);
	RayIn(shaderSS);
	Value(shaderSS);
	RayFunc(shaderSS);

	return shaderSS.str();
}

/*
void FS_Generator::Data(stringstream & shaderSS) {
	auto const & sceneData = hitableVisitor->GetSceneData();
	shaderSS << "const float SceneData[" << sceneData.size() << "] = float[](" << endl;

	if (!sceneData.empty()) {
		shaderSS << "    " << sceneData[0];
		for (size_t i = 1; i < sceneData.size(); i++) {
			shaderSS << ", ";
			if ((i + 1) % 16 == 0)
				shaderSS << endl << "    ";
			shaderSS << sceneData.at(i);
		}
	}

	shaderSS << endl << ");" << endl << "" << endl;

	
	auto const & matData = matVisitor->GetMatData();
	shaderSS << "const float MatData[" << matData.size() << "] = float[](" << endl;
	if (!matData.empty()) {
		shaderSS << matData[0];
		for (size_t i = 1; i < matData.size(); i++) {
			shaderSS << ", ";
			if ((i + 1) % 16 == 0)
				shaderSS << endl << "    ";
			shaderSS << matData.at(i);
		}
	}
	shaderSS << endl << ");" << endl << "" << endl;


	auto const & texData = texVisitor->GetTexData();
	shaderSS << "const float TexData[" << texData.size() << "] = float[](" << endl;
	if (!texData.empty()) {
		shaderSS << texData[0];
		for (size_t i = 1; i < texData.size(); i++) {
			shaderSS << ", ";
			if ((i + 1) % 16 == 0)
				shaderSS << endl << "    ";
			shaderSS << texData.at(i);
		}
	}
	shaderSS << endl << ");" << endl << "" << endl;
}
*/

void FS_Generator::Version(stringstream & shaderSS) {
	shaderSS << "#version 330 core" << endl
		<< "" << endl;
}

void FS_Generator::LayOut(stringstream & shaderSS) {
	shaderSS
		<< "layout (location = 0) out vec4 out_origin_curRayNum;" << endl
		<< "layout (location = 1) out vec4 out_dir_tMax;" << endl
		<< "layout (location = 2) out vec4 out_color_time;" << endl
		<< "layout (location = 3) out vec3 out_rayTracingRst;" << endl
		<< "" << endl;
}

void FS_Generator::Struct(stringstream & shaderSS) {
	shaderSS
		<< "struct Camera{" << endl
		<< "    vec3 pos;" << endl
		<< "    vec3 BL_Corner;" << endl
		<< "    vec3 horizontal;" << endl
		<< "    vec3 vertical;" << endl
		<< "    vec3 right;" << endl
		<< "    vec3 up;" << endl
		<< "    vec3 front;" << endl
		<< "    float lenR;" << endl
		<< "    float t0;" << endl
		<< "    float t1;" << endl
		<< "};" << endl
		<< "void Camera_GenRay();" << endl
		<< "" << endl
		<< "struct Ray{" << endl
		<< "    vec3 origin;" << endl
		<< "    vec3 dir;" << endl
		<< "    highp vec3 color;" << endl
		<< "    float tMax;" << endl
		<< "    float time;" << endl
		<< "    highp float curRayNum;//这里使用float以统一" << endl
		<< "};" << endl
		<< "void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation);" << endl
		<< "" << endl
		<< "struct Vertex{" << endl
		<< "    vec3 pos;" << endl
		<< "    vec3 normal;" << endl
		<< "    vec2 uv;" << endl
		<< "};" << endl
		<< "struct Vertex Vertex_InValid = struct Vertex(vec3(0), vec3(0), vec2(0));" << endl
		<< "" << endl
		<< "struct HitRst{" << endl
		<< "    bool hit;" << endl
		<< "    struct Vertex vertex;" << endl
		<< "    int matIdx;" << endl
		<< "};" << endl
		<< "struct HitRst HitRst_InValid = struct HitRst(false, Vertex_InValid, -1);" << endl
		<< "" << endl;
}

void FS_Generator::Uniform(stringstream & shaderSS) {
	shaderSS
		<< "uniform sampler2D origin_curRayNum;" << endl
		<< "uniform sampler2D dir_tMax;//若tMax为0, 则表示该光线无效" << endl
		<< "uniform sampler2D color_time;" << endl
		<< "uniform sampler2D rayTracingRst;" << endl
		<< "" << endl
		<< "uniform sampler2D SceneData;" << endl
		<< "uniform sampler2D MatData;" << endl
		<< "uniform sampler2D TexData;" << endl
		<< "" << endl
		<< "uniform struct Camera camera;" << endl
		<< "uniform float rdSeed[4];" << endl
		<< "uniform float RayNumMax;" << endl
		<< "" << endl;
}

void FS_Generator::Variable(stringstream & shaderSS) {
	shaderSS
		<< "const float PI = 3.1415926;" << endl
		<< "const float tMin = 0.001;" << endl
		<< "const float FLT_MAX = 99999999999999999999999999999999999999.0;" << endl
		<< "const float rayP = 50.0/51.0;// depth = p/(1-p) --> p = depth/(depth+1)" << endl
		<< "const int HT_Sphere         = 0;" << endl
		<< "const int HT_Group          = 1;" << endl
		<< "const int MatT_Lambertian   = 0;" << endl
		<< "const int MatT_Metal        = 1;" << endl
		<< "const int MatT_Dielectric   = 2;" << endl
		<< "const int TexT_ConstTexture = 0;" << endl
		<< "" << endl
		<< "int rdCnt = 0;" << endl
		<< "in vec2 TexCoords;" << endl
		<< "struct Ray gRay;" << endl
		<< "" << endl;
}

void FS_Generator::FuncDefine(stringstream & shaderSS) {
	shaderSS
		<< "float RandXY(float x, float y);// [0.0, 1.0)" << endl
		<< "float Rand();// [0.0, 1.0)" << endl
		<< "vec2 RandInSquare();" << endl
		<< "vec2 RandInCircle();" << endl
		<< "vec3 RandInSphere();" << endl
		<< "float At(sampler2D data, int idx);" << endl
		<< "float atan2(float y, float x);" << endl
		<< "vec2 Sphere2UV(vec3 normal);" << endl
		<< "float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);" << endl
		<< "void SetRay();" << endl
		<< "void WriteRay(int mode);" << endl
		<< "struct HitRst RayIn_Scene();" << endl
		<< "struct HitRst RayIn_Sphere(int idx);" << endl
		<< "bool Scatter_Material(struct Vertex vertex, int matIdx);" << endl
		<< "bool Scatter_Lambertian(struct Vertex vertex, int matIdx);" << endl
		<< "bool Scatter_Metal(struct Vertex vertex, int matIdx);" << endl
		<< "bool Scatter_Dielectric(struct Vertex vertex, int matIdx);" << endl
		<< "vec3 Value_Texture(vec2 uv, vec3 p, int texIdx);" << endl
		<< "vec3 Value_ConstTexture(int texIdx);" << endl
		<< "void RayTracer();" << endl
		<< "" << endl;
}

void FS_Generator::Main(std::stringstream & shaderSS) {
	shaderSS
		<< "void main(){" << endl
		<< "    RayTracer();" << endl
		<< "}" << endl
		<< "" << endl;
}

void FS_Generator::RandFunc(stringstream & shaderSS) {
	shaderSS // RandXY
		<< "float RandXY(float x, float y){" << endl
		<< "    return fract(cos(x * (12.9898) + y * (4.1414)) * 43758.5453);" << endl
		<< "}" << endl
		<< " " << endl;

	shaderSS // Rand
		<< "float Rand(){" << endl
		<< "    float a = RandXY(TexCoords.x, rdSeed[0]);" << endl
		<< "    float b = RandXY(rdSeed[1], TexCoords.y);" << endl
		<< "    float c = RandXY(rdCnt++, rdSeed[2]);" << endl
		<< "    float d = RandXY(rdSeed[3], a);" << endl
		<< "    float e = RandXY(b, c);" << endl
		<< "    float f = RandXY(d, e);" << endl
		<< " " << endl
		<< "    return f;" << endl
		<< "}" << endl
		<< " " << endl;

	shaderSS // RandInSquare
		<< "vec2 RandInSquare(){" << endl
		<< "    return vec2(Rand(), Rand());" << endl
		<< "}" << endl
		<< " " << endl;

	shaderSS // RandInCircle
		<< "vec2 RandInCircle(){" << endl
		<< "    vec2 rst;" << endl
		<< "    do {" << endl
		<< "        rst = vec2(Rand(), Rand())*2.0f - 1.0f;" << endl
		<< "    } while (dot(rst, rst) >= 1.0);" << endl
		<< "    return rst;" << endl
		<< "}" << endl
		<< " " << endl;

	shaderSS // RandInSphere
		<< "vec3 RandInSphere(){" << endl
		<< "    vec3 rst;" << endl
		<< "    do {" << endl
		<< "        rst = vec3(Rand(), Rand(), Rand())*2.0f - 1.0f;" << endl
		<< "    } while (dot(rst, rst) >= 1.0);" << endl
		<< "    return rst;" << endl
		<< "}" << endl;
}

void FS_Generator::MathFunc(stringstream & shaderSS) {
	shaderSS // At
		<< "float At(sampler2D data, int idx){" << endl
		<< "    vec2 texCoords = vec2(float(idx)/textureSize(data, 0).x, 0);" << endl
		<< "    float val = texture2D(data, texCoords).x;" << endl
		<< "    return val;" << endl
		<< "}" << endl
		<< " " << endl;

	shaderSS // atan2
		<< "float atan2(float y, float x){" << endl
		<< "    if(x>0){" << endl
		<< "        return atan(y/x);" << endl
		<< "    }" << endl
		<< "    else if(x<0){" << endl
		<< "        if(y>=0){" << endl
		<< "            return atan(y/x) + PI;" << endl
		<< "        }else{" << endl
		<< "            return atan(y/x) - PI;" << endl
		<< "        }" << endl
		<< "    }" << endl
		<< "    else{// x==0" << endl
		<< "        return sign(y) * PI / 2;" << endl
		<< "    }" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // Sphere2UV
		<< "vec2 Sphere2UV(vec3 normal) {" << endl
		<< "    vec2 uv;" << endl
		<< "    float phi = atan2(normal.z, normal.x);" << endl
		<< "    float theta = asin(normal.y);" << endl
		<< "    uv[0] = 1 - (phi + PI) / (2 * PI);" << endl
		<< "    uv[1] = (theta + PI/2) / PI;" << endl
		<< "    return uv;" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // FresnelSchlick
		<< "float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi){" << endl
		<< "    float cosTheta = dot(viewDir, halfway);" << endl
		<< "    float R0 = pow((ratioNtNi - 1) / (ratioNtNi + 1), 2);" << endl
		<< "    float R = R0 + (1 - R0)*pow(1 - cosTheta, 5);" << endl
		<< "    return R;" << endl
		<< "}" << endl
		<< "" << endl;
}

void FS_Generator::Scatter(stringstream & shaderSS) {
	shaderSS // Scatter_Material
		<< "bool Scatter_Material(struct Vertex vertex, int matIdx){" << endl
		<< "    int matType = int(At(MatData, matIdx+1));" << endl
		<< "    " << endl
		<< "    if(matType == MatT_Lambertian)" << endl
		<< "        return Scatter_Lambertian(vertex, matIdx);" << endl
		<< "    else if(matType == MatT_Metal)" << endl
		<< "        return Scatter_Metal(vertex, matIdx);" << endl
		<< "    else if(matType == MatT_Dielectric)" << endl
		<< "        return Scatter_Dielectric(vertex, matIdx);" << endl
		<< "    else{" << endl
		<< "        gRay.color = vec3(1,0,1);//以此提示材质存在问题" << endl
		<< "        return false;" << endl
		<< "    }" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // Scatter_Lambertian
		<< "bool Scatter_Lambertian(struct Vertex vertex, int matIdx){" << endl
		<< "    int texIdx = int(At(MatData, matIdx+1));" << endl
		<< "    vec3 albedo = Value_Texture(vertex.uv, vertex.pos, texIdx);" << endl
		<< "    " << endl
		<< "    gRay.dir = vertex.normal + RandInSphere();" << endl
		<< "    gRay.origin = vertex.pos;" << endl
		<< "    gRay.color *= albedo;" << endl
		<< "    gRay.tMax = FLT_MAX;" << endl
		<< "    return true;" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // Scatter_Metal
		<< "bool Scatter_Metal(struct Vertex vertex, int matIdx){" << endl
		<< "    int texIdx = int(At(MatData, matIdx+1));" << endl
		<< "    vec3 specular = Value_Texture(vertex.uv, vertex.pos, texIdx);" << endl
		<< "    float fuzz = At(MatData, matIdx+4);" << endl
		<< "    " << endl
		<< "    vec3 dir = reflect(gRay.dir, vertex.normal);" << endl
		<< "    vec3 dirFuzz = dir + fuzz * RandInSphere();" << endl
		<< "    " << endl
		<< "    // 反射光线在表面之下" << endl
		<< "    if (dot(dirFuzz, vertex.normal) < 0) {" << endl
		<< "        gRay.color = vec3(0);" << endl
		<< "        return false;" << endl
		<< "    }" << endl
		<< "    " << endl
		<< "    Ray_Update(vertex.pos, dirFuzz, specular);" << endl
		<< "    return true;" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // Scatter_Dielectric
		<< "bool Scatter_Dielectric(struct Vertex vertex, int matIdx){" << endl
		<< "    float refractIndex = At(MatData, matIdx+1);" << endl
		<< "    " << endl
		<< "    vec3 refractDir;" << endl
		<< "    vec3 reflectDir = reflect(gRay.dir, vertex.normal);" << endl
		<< "    " << endl
		<< "    vec3 ud = normalize(gRay.dir);" << endl
		<< "    vec3 un = normalize(vertex.normal);" << endl
		<< "    vec3 airViewDir;" << endl
		<< "    if (dot(ud,un) < 0) {//外部" << endl
		<< "        refractDir = refract(ud, un, 1.0f / refractIndex);" << endl
		<< "        airViewDir = -ud;" << endl
		<< "    }" << endl
		<< "    else {//内部" << endl
		<< "        refractDir = refract(ud, -un, refractIndex);" << endl
		<< "        if (refractDir == vec3(0)) {" << endl
		<< "            Ray_Update(vertex.pos, reflectDir, vec3(1));" << endl
		<< "            return true;" << endl
		<< "        }" << endl
		<< "        " << endl
		<< "        airViewDir = refractDir;" << endl
		<< "    }" << endl
		<< "    " << endl
		<< "    float fresnelFactor = FresnelSchlick(airViewDir, un, refractIndex);" << endl
		<< "    vec3 dir = Rand() > fresnelFactor ? refractDir : reflectDir;" << endl
		<< "    Ray_Update(vertex.pos, dir, vec3(1));" << endl
		<< "    return true;" << endl
		<< "}" << endl
		<< "" << endl;
}

void FS_Generator::RayIn(stringstream & shaderSS) {
	shaderSS // RayIn_Scene
		<< "struct HitRst RayIn_Scene(){" << endl
		<< "    int idxStack[100];" << endl
		<< "    int idxStackSize = 0;" << endl
		<< "    struct HitRst finalHitRst = HitRst_InValid;" << endl
		<< "    " << endl
		<< "    idxStack[idxStackSize++] = 0;" << endl
		<< "    while(idxStackSize > 0){" << endl
		<< "        int idx = idxStack[--idxStackSize];" << endl
		<< "        " << endl
		<< "        int type = int(At(SceneData, idx));//32b float 在 1677w 时出现误差, 故可接受" << endl
		<< "        if(type == HT_Sphere){" << endl
		<< "            struct HitRst hitRst = RayIn_Sphere(idx);" << endl
		<< "            if(hitRst.hit)" << endl
		<< "                finalHitRst = hitRst;" << endl
		<< "        }" << endl
		<< "        else if(type == HT_Group){" << endl
		<< "            int childrenNum = int(At(SceneData, idx+3));" << endl
		<< "            for(int i=0; i < childrenNum; i++)" << endl
		<< "                idxStack[idxStackSize++] = int(At(SceneData, idx+4+i));" << endl
		<< "        }" << endl
		<< "        //else" << endl
		<< "        //    ;// do nothing" << endl
		<< "    }" << endl
		<< "    " << endl
		<< "    return finalHitRst;" << endl
		<< "}" << endl
		<< "" << endl;


	shaderSS // RayIn_Sphere
		<< "struct HitRst RayIn_Sphere(int idx){" << endl
		<< "    int matIdx = int(At(SceneData, idx+1));//32b float 在 1677w 时出现误差, 故可接受" << endl
		<< "    vec3 center = vec3(At(SceneData, idx+3), At(SceneData, idx+4), At(SceneData, idx+5));" << endl
		<< "    float radius = At(SceneData, idx+6);" << endl
		<< "    " << endl
		<< "    struct HitRst hitRst;" << endl
		<< "    hitRst.hit = false;" << endl
		<< "    " << endl
		<< "    vec3 oc = gRay.origin - center;" << endl
		<< "    float a = dot(gRay.dir, gRay.dir);" << endl
		<< "    float b = dot(oc, gRay.dir);" << endl
		<< "    float c = dot(oc, oc) - radius * radius;" << endl
		<< "    float discriminant = b * b - a * c;" << endl
		<< "    " << endl
		<< "    if (discriminant <= 0)" << endl
		<< "        return hitRst;" << endl
		<< "    " << endl
		<< "    float t = (-b - sqrt(discriminant)) / a;" << endl
		<< "    if (t > gRay.tMax || t < tMin) {" << endl
		<< "        t = (-b + sqrt(discriminant)) / a;" << endl
		<< "        if (t > gRay.tMax || t < tMin)" << endl
		<< "            return hitRst;" << endl
		<< "    }" << endl
		<< "    " << endl
		<< "    gRay.tMax = t;" << endl
		<< "    " << endl
		<< "    hitRst.hit = true;" << endl
		<< "    hitRst.vertex.pos = gRay.origin + t * gRay.dir;" << endl
		<< "    hitRst.vertex.normal = (hitRst.vertex.pos - center) / radius;" << endl
		<< "    hitRst.vertex.uv = Sphere2UV(hitRst.vertex.normal);" << endl
		<< "    hitRst.matIdx = matIdx;" << endl
		<< "    " << endl
		<< "    return hitRst;" << endl
		<< "}" << endl
		<< " " << endl;
}

void FS_Generator::Value(stringstream & shaderSS) {
	shaderSS // Value_Texture
		<< "vec3 Value_Texture(vec2 uv, vec3 p, int texIdx){" << endl
		<< "	int type = int(At(SceneData, texIdx));" << endl
		<< "	if(type == TexT_ConstTexture){" << endl
		<< "		return Value_ConstTexture(texIdx);" << endl
		<< "	}else" << endl
		<< "		return vec3(1,0,1);" << endl
		<< "}" << endl
		<< " " << endl;
	
	shaderSS // Value_ConstTexture
		<< "vec3 Value_ConstTexture(int texIdx){" << endl
		<< "	vec3 color = vec3(At(SceneData, texIdx+1), At(SceneData, texIdx+2), At(SceneData, texIdx+3));" << endl
		<< "	return color;" << endl
		<< "}" << endl
		<< "" << endl;
}

void FS_Generator::RayFunc(stringstream & shaderSS) {
	shaderSS // Ray_Update
		<< "void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation){" << endl
		<< "    gRay.origin = origin;" << endl
		<< "    gRay.dir = dir;" << endl
		<< "    gRay.color *= attenuation;" << endl
		<< "    gRay.tMax = FLT_MAX;" << endl
		<< "}" << endl
		<< "" << endl;
	
	shaderSS // Camera_GenRay
		<< "void Camera_GenRay(){" << endl
		<< "    vec2 st = TexCoords + RandInSquare() / textureSize(origin_curRayNum, 0);" << endl
		<< "    vec2 rd = camera.lenR * RandInCircle();" << endl
		<< "    " << endl
		<< "    gRay.origin = camera.pos + rd.x * camera.right + rd.y * camera.up;" << endl
		<< "    gRay.dir = camera.BL_Corner + st.s * camera.horizontal + st.t * camera.vertical - gRay.origin;" << endl
		<< "    gRay.color = vec3(1);" << endl
		<< "    gRay.tMax = FLT_MAX;" << endl
		<< "    gRay.time = mix(camera.t0, camera.t1, Rand());" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // SetRay
		<< "void SetRay(){" << endl
		<< "    vec4 val_origin_curRayNum = texture(origin_curRayNum, TexCoords);" << endl
		<< "    gRay.curRayNum = val_origin_curRayNum.w;" << endl
		<< "    if(gRay.curRayNum >= RayNumMax)" << endl
		<< "        return;" << endl
		<< "    vec4 val_dir_tMax = texture(dir_tMax, TexCoords);" << endl
		<< "    if(val_dir_tMax.w == 0){" << endl
		<< "        Camera_GenRay();" << endl
		<< "    }else{" << endl
		<< "        gRay.origin = val_origin_curRayNum.xyz;" << endl
		<< "        gRay.dir = val_dir_tMax.xyz;" << endl
		<< "        gRay.tMax = val_dir_tMax.w;" << endl
		<< "        vec4 val_color_time = texture(color_time, TexCoords);" << endl
		<< "        gRay.color = val_color_time.xyz;" << endl
		<< "        gRay.time = val_color_time.w;" << endl
		<< "    }" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // WriteRay
		<< "void WriteRay(int mode){" << endl
		<< "    vec3 color = texture(rayTracingRst, TexCoords).xyz;" << endl
		<< "" << endl
		<< "    if(mode == 0){//最终没有击中光源" << endl
		<< "        gRay.tMax = 0;" << endl
		<< "        color *= gRay.curRayNum / (gRay.curRayNum + 1);" << endl
		<< "        gRay.curRayNum = min(gRay.curRayNum + 1, RayNumMax);" << endl
		<< "    }" << endl
		<< "    else if(mode == 1){//击中光源" << endl
		<< "        gRay.tMax = 0;" << endl
		<< "        color = (color * gRay.curRayNum + gRay.color) / (gRay.curRayNum + 1);" << endl
		<< "        gRay.curRayNum = min(gRay.curRayNum + 1, RayNumMax);" << endl
		<< "    }" << endl
		<< "    //else if(mode == 2){//继续追踪" << endl
		<< "    //  //do nothing" << endl
		<< "    //}" << endl
		<< "    else if(mode == 3){" << endl
		<< "        gRay.curRayNum = RayNumMax + 100;" << endl
		<< "    }else" << endl
		<< "        ;//do nothing" << endl
		<< "" << endl
		<< "    out_origin_curRayNum = vec4(gRay.origin, gRay.curRayNum);" << endl
		<< "    out_dir_tMax = vec4(gRay.dir, gRay.tMax);" << endl
		<< "    out_color_time = vec4(gRay.color, gRay.time);" << endl
		<< "    out_rayTracingRst = color;" << endl
		<< "}" << endl
		<< "" << endl;

	shaderSS // RayTracer
		<< "void RayTracer(){" << endl
		<< "    SetRay();" << endl
		<< "    if(gRay.curRayNum >= RayNumMax){" << endl
		<< "        WriteRay(3);" << endl
		<< "        return;" << endl
		<< "    }" << endl
		<< "    " << endl
		<< "    if(Rand() > rayP){" << endl
		<< "        WriteRay(0);//最终没有击中光源" << endl
		<< "        return;" << endl
		<< "    }" << endl
		<< "    " << endl
		<< "    struct HitRst finalHitRst = RayIn_Scene();" << endl
		//<< "    if(false){" << endl
		<< "    if(finalHitRst.hit){" << endl
		<< "        bool rayOut = Scatter_Material(finalHitRst.vertex, finalHitRst.matIdx);" << endl
		<< "        " << endl
		<< "        if(rayOut)" << endl
		<< "            WriteRay(2);//继续追踪" << endl
		<< "        else" << endl
		<< "            WriteRay(1);" << endl
		<< "    }else{// sky" << endl
		<< "        float t = 0.5f * (normalize(gRay.dir).y + 1.0f);" << endl
		<< "        vec3 white = vec3(1.0f, 1.0f, 1.0f);" << endl
		<< "        vec3 blue = vec3(0.5f, 0.7f, 1.0f);" << endl
		<< "        vec3 lightColor = (1 - t) * white + t * blue;" << endl
		<< "        gRay.color *= lightColor;" << endl
		<< "        WriteRay(1);" << endl
		<< "    }" << endl
		<< "}" << endl
		<< "" << endl;
}
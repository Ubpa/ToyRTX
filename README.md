# 1. 简介

这是我对 光线追踪 的练习，内容基于

- Ray Tracing in One Weekend
- Ray Tracing the Next Week
- Ray Tracing the Rest of Your Life

> 作者的源码网址
>
> https://github.com/petershirley/raytracinginoneweekend
>
> https://github.com/petershirley/raytracingthenextweek
>
> https://github.com/petershirley/raytracingtherestofyourlife

对内容进行了一定的修改，图形接口使用的是 OpenGL

# 2. 内容

```
/01_in_One_Weekend
  -01_01_Image
  -01_02_Camera
  -01_03_Sphere
  -01_04_Group
  -01_05_Material
  -01_06_Scene
/Utility
  /OpenGL
    -Camera
    -FBO
    -gal
    -Glfw
    -Mesh
    -Model
    -Shader
    -Texture
    -VAO
  /Other
    -ArgManager
    -Config
    -EventManager
    -File
    -Header: Array2D, GStorage, LStorage, Pool, Ptr, RandSet, Singleton, Vec
    -HeapObj
    -Image
    -Math
    -Operation: Operation, LambdaOp, InfoLambdaOp, OpQueue
    -Shape: Shape, Cube, Sphere
    -Timer
  /RayTracing
    -Hitable: Group, Hitable, Sky, Spheres
    -ImgWindow
    -Material: Dielectric, Lambertian, Material, Metal, OpMaterial
    -Ray: Ray, TRay
    -RayCamera: RayCamera, TRayCamera
    -Scene
```

# 2. 使用方法

## 2.1 需求软件

- Visual Studio 2017（其他版本没有测试）
- Git
- CMake（版本2.8以上）

## 2.2 下载代码

```bash
git clone https://github.com/Ubpa/RayTracingToy
```

## 2.3 编译

1. 建立工程

```bash
# 在 cmd 中运行如下命令
mkdir build
cd build
cmake ..
```

2. 打开 `/build/RayTracingToy.sln`
3. 在**解决方案中**的`CMakePredefinedTargets`中，右键`INSTALL`，点击生成

## 2.4 配置

在运行exe前，可以对config.out进行修改来配置，在调试时在config.in进行修改来配置
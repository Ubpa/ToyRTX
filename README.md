# 1. 简介

C++ 相关工具

# 2. 内容

```
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
  -Header: Slogleton, Storage, Pool, Vec, Array2D 
  -Image
  -Operation: Operation, LambdaOp, InfoLambdaOp, OpQueue
  -Shape: Shape, Cube, Sphere
  -Timer
```

# 2. 使用方法

## 2.1 需求软件

- Visual Studio 2017（其他版本没有测试）
- Git
- CMake

## 2.2 下载代码

```bash
git clone https://github.com/Ubpa/LearnOpenGL
```

## 2.3 编译

1. 建立工程

```bash
# 在 cmd 中运行如下命令
mkdir build
cd build
cmake ..
```

2. 打开 `/build/LearnOpenGL.sln`

3. 在**解决方案中**的`CMakePredefinedTargets`中，右键`INSTALL`，点击生成

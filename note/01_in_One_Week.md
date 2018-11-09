# 1. Ray Tracing in One Weekend

## 1.1 Output an Image

**PPM格式**

```
P3
# The P3 means colors are in ASCII, then 3 columns and 2 rows,
# width == 3, height == 2
# then 255 for max color, then RGB triplets
3 2
255
255	0	0
0	255	0
0	0	255
255	255	0
255	255	255
0	0	0
```

**图像库**

`ppm`格式以`ASCII`存储，文件较大，考虑采用图像的无损压缩格式`png`来存储图像。

可使用开源库 `stb_image` 和 `stb_image_write` 来读写图像

> https://github.com/nothings/stb

**图像实时显示**

如果只使用图像来检查结果，则需等待计算完毕；而光线追踪算法耗时较长，故考虑实时显示。

图形接口使用的是OpenGL，我将功能封装在了 `Utility/RayTracing/ImgWindow` 中。

**实现**

```c++
int main(int argc, char ** argv) {
	ImgWindow imgWindow(str_WindowTitle);
	Image & img = imgWindow.GetImg();
	const size_t val_ImgWidth = img.GetWidth();
	const size_t val_ImgHeight = img.GetHeight();
	const size_t val_ImgChannel = img.GetChannel();

	auto imgUpdate = Operation::ToPtr(new LambdaOp([&]() {
		static int f = 0;
		for (size_t i = 0; i < img.GetWidth(); i++) {
			for (size_t j = 0; j < img.GetHeight(); j++) {
				float r = 0.5 * i / (float)img.GetWidth();
				float g = 0.5 * j / (float)img.GetHeight();
				float b = 0.2 + 0.2*sinf(0.01f * f);
				img.SetPixel(i, j, Image::Pixel<float>(r, g, b));
			}
		}
		f++;
	}, true));

	imgWindow.Run(imgUpdate);

	return 0;
}
```

结果输出在 `data/out` 中

图像的(0,0)在左上角，右为x轴，下为y轴

OpenGL中，图像的(0,0)在左下角，右为x轴，上为y轴

## 1.2 Ray, Camera, Background

**相机**

相机朝向像平面的中心，相机的位置处于焦点处

相机到成像平面的距离为焦距

相机的右轴为像平面的横轴，相机的上轴为像平面的纵轴

## 1.3 Sphere

**相交**
$$
|\mathbf{P}-\mathbf{C}|=R\\
\mathbf{P}=\mathbf{A}+t\mathbf{B}\\
(t\mathbf{B}+\mathbf{A}-\mathbf{C})^2=R^2\\
|\mathbf{B}|^2t^2+2\ dot(\mathbf{B},\ \mathbf{A}-\mathbf{C})\ t+|\mathbf{A}-\mathbf{C}|^2-R^2=0
$$

## 1.4 Surface normals and multiple objects


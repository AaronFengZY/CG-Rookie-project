# 渲染过程概述

渲染过程可能是三个实验中最复杂的一个，因为它涉及到多个坐标系的变换。下面是一个简要的总结：

**坐标系变换流程**:
- 物体坐标系 **--(model)-->** 世界坐标系
- 世界坐标系 **--(view)-->** 相机坐标系
- 相机坐标系 **--(projection)-->** 裁剪坐标系 (2D)
- 裁剪坐标系 **--(视口变换)-->** 屏幕

**深度测试**：
- 非常重要的一环，一定要使用裁剪坐标系的z值来进行深度测试。

**其他注意事项**：
- 按照实验文档进行操作。

**`rasterizer.cpp` 关键功能**:
- `Rasterizer::inside_triangle`: 判断给定像素坐标 `(x,y)` 是否在三角形内部。这在光栅化时需要判断当前像素坐标是否在当前三角形片中。
- `Rasterizer::compute_barycentric_2d`: 计算给定像素坐标 `(x,y)` 对应的三角形重心坐标 `[alpha, beta, gamma]`。这在进行透视矫正时使用。
- `Rasterizer::draw`: 对当前渲染物体的所有三角形面片进行遍历，进行几何变换（调用`vertex_shader`）以及光栅化（调用 `rasterize_triangle`）。
- `Rasterizer::rasterize_triangle`: 对当前三角形进行光栅化，进行深度测试（检查`depth_buffer`），并调用 `phong_fragment_shader`。

**`shader.cpp` 关键功能**:
- `vertex_shader`: 将顶点坐标变换到投影平面，进行视口变换；同时将法线向量变换到相机坐标系用于后续插值。
- `phong_fragment_shader`: 使用 Blinn-Phong 模型计算每个片元（像素）的颜色。

**额外注意事项**：
- 对法向量进行标准化（normalized）处理。

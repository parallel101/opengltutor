# 现代 OpenGL 保姆级教程

零基础入门计算机图形学必不可少的在线网络公开课，手把手教您现代 OpenGL 的点点滴滴，构建爆款游戏引擎。

本仓库仅仅包含实验源码，课件下载请前往：https://github.com/parallel101/openglslides

国内镜像下载：https://gitee.com/archibate/opengltutor

> 本分支为最新一课（第三课）的作业项目源码，[要做第一课作业的同学请点击这里跳转到 hw01 分支](https://github.com/parallel101/opengltutor/tree/hw01)。提交 PR 时，请在标题中写明是第几课的作业，对于第一课的作业，则应该将目标分支设为 hw01。

## 课程简介

你是否想要掌握计算机图形学的核心原理和技术？你是否想要利用现代OpenGL创建自己的3D游戏引擎？但又苦于没有简单易懂适合入门的中文教程？如果是，那么这门课程就是为你量身定制的！ 在这门课程中，你将从基础知识开始，逐步深入探索图形渲染管线的各个阶段，学习如何使用OpenGL和GLSL进行高效的图形编程，实现各种真实感效果，如光照、纹理、阴影等。你还将动手搭建一个完整的3D游戏引擎框架，体验从模型导入、场景管理、相机控制到碰撞检测、动画系统等各个方面的设计和实现。通过这门课程，你将获得丰富的图形学理论和实践知识，为你未来的图形学创作和研究打下坚实的基础。 这门网络公开课每周六2点开始直播，每次约1小时，共15课，错过了也不要紧，每一期的录播都会上传到B站免费观看。

目标：打造一款基于 OpenGL 的 3D 游戏引擎，开发出爆款开源游戏。

面向人群：有一定编程基础，学过 C 语言，想要入门计算机图形学，OpenGL 的初学者。

能学到的东西：现代 OpenGL API 的使用，线性代数与矢量微积分，图形管线的原理，GLSL 着色器语言，迪士尼 BRDF、TAA、IBL、全局光照等现代渲染技术，游戏引擎的 ECS 架构，现代 C++ 设计模式，软件工程最佳实践，软件跨平台，部署与发布，多线程性能优化等。

直播间：https://live.bilibili.com/14248205

课程录播：https://space.bilibili.com/263032155/channel/collectiondetail?sid=53025

## 课程大纲

1. 从配置安装到画第一个三角形（BV1Na4y1c7tP）
2. 重学线性代数矢量与矩阵（BV1ej411U7SW）
3. 三维模型的加载与相机控制 (录播文件丢失，正在重置中)
4. GLSL 着色器语言与 PBR 光照模型
5. UV、法线与材质贴图的加载和使用
6. 离屏渲染与点选物体的实现
7. 高质量实时软阴影的实现
8. 环境光贴图与 IBL 烘培
9. 色调映射、延迟渲染、Blooming 与 TAA
10. 屏幕空间反射与 SDF 全局光照
11. 几何着色器：实例化与曲面细分
12. 骨骼动画与蒙皮：角色走路动画的实现
13. 地型的程序化生成与天空体积云的渲染
14. 用计算着色器做实时物理仿真
15. 游戏引擎 ECS 架构的设计与实现

## 课程参考资源

- Learn OpenGL 网站（非常适合初学者）：learnopengl.com
- 红宝书：《OpenGL Programming Guide》或称《OpenGL编程指南》
- 蓝宝书：《OpenGL Superbible: Comprehensive Tutorial and Reference》
- 着色器：《OpenGL 4.0 Shading Language Cookbook》
- GPU Gems 系列：https://developer.nvidia.com/gpugems/gpugems3/foreword
- OpenGL 官方主页：http://www.opengl.org/
- Khronos（OpenGL 的维护团队）主页：http://www.khronos.org/
- GAMES104（小彭老师所对标的课程）：https://games104.boomingtech.com
- 《游戏引擎架构》：https://book.douban.com/subject/25815142
- 小彭老师课程群（不上课的同学不要加）：458712187

## 硬件要求

流畅运行所需最低配置：
- 显卡：Intel 或 AMD 集成显卡，支持 OpenGL 4.3
- 处理器：2 GHz 以上处理器，64 位
- 内存：4 GB
- 硬盘：20 GB 空闲空间

小彭老师同款配置：
- 显卡：NVIDIA GeForce RTX 2080（或 AMD Radeon RX 5700 XT）独立显卡
- 处理器：Intel Core i7-9750H（或 AMD Ryzen 7 4800H）
- 内存：32 GB（DDR4）
- 硬盘：1 TB

## 软件要求

编译源码所需软件：
- 操作系统：Windows >= 10 或 Ubuntu >= 20.04 或 MSYS2 或 WSL2（需要安装 WSLg）
- 编译器：MSVC >= 19 或 GCC >= 9 或 Clang >= 11（支持 C++17 即可）
- 编辑器：Visual Studio 2019、2022 或 VS Code 等任意你用得惯的编辑器
- 构建系统：CMake >= 3.10

小彭老师所用软件：
- 操作系统：Arch Linux
- 编译器：GCC 12.2.1
- 编辑器：NeoVim 0.9.1
- 构建系统：CMake 3.26

## Windows 开发环境搭建

网盘分享链接：https://pan.baidu.com/s/1TZ6nVJC7DZIuUarZrGJYow
提取码：opgl

内含：

1. VS2022（自带了 CMake 和 Git）
1. DX 运行时（包含 OpenGL）
1. OpenGL 软光栅（你没有显卡时才需要下载）
1. CPU-Z 配置检测工具（可查看显卡和处理器型号）

> 仅供方便，不一定非用网盘里的文件不可，也可以从网上搜索其他资源下载，也可以选择其他 IDE。

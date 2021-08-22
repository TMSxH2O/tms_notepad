> OpenGL Shading Language Cookbook<br>主要围绕的是 OpenGL Shading Language，即 GLSL进行介绍

# 概要
OpenGL 环境的准备，以及涉及到各个库的很多用法小技巧。

## GLM
OpenGL或者说是专门用于 GLSL 的数学库，其中为 GLSL 的使用提供了很多便利。包括封装了很多常见的计算以及数据结构。

### GLM to GLSL
GLM 中的数据到 GLSL。GLSL 接受的，通常是 `float` 或者 `int` 类型的指针，GLM 中有很多封装的数据结构，其底层就对应这些数据类型。

如，希望传入一个矩阵数据类型，就需要使用 `&mat[0][0]` 或者也可以使用 GLM 的方法 `glm::value_ptr(mat)` 传入指针数据。

## 
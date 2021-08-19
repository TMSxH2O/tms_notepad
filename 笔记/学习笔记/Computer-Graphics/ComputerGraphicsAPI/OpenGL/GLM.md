> GLM 是一款专门为OpenGL开发的开发数学库，用于完成OpenGL中必然会涉及到的很多数学相关的操作

# 常见方法

列举GLM中经常会使用到的方法

## 基础

GLM中最基础，最常用的方法

### 向量

- `normalize` 标准化向量，将向量转换为单位向量
- `cross` 向量叉乘，也可以使用 `dot`

### 矩阵

- `determainant` 求解矩阵的行列式
- `inverse` 计算矩阵的逆矩阵
- `transpost` 计算矩阵的转置

## 投影矩阵

> `glm/gtc/matrix_transform.hpp`

- `ortho` 正交投影
- `projective` 透视投影

## 光线

- `reflect` 计算向量基于法线的反射（此处传入的是入射光，因此方向应该是指向交点）

# 添加宏定义
```cmake
# 其中 -D 之后，跟需要的宏定义，如 -DWIN32 表示添加了 WIN32 宏定义
add_definitions(-Dxxx)
```

# 添加路径
效果类似 VS 中添加了 `VS++ 目录` 中的 `包含目录`

```cmake
# 其中支持两种形式，相对路径对应双引号，绝对路径对应尖括号
include_directories(
	./include
	F:/src/include
)
```

# 添加链接库
通过 `target_link_directories` 来添加库所在的目录后，使用 `target_link_libraries` 来引入对应的库文件。

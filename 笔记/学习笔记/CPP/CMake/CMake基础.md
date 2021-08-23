CMake的基本使用记录，可以配合 Python 脚本的 `cmake-converter` 来进行学习使用。

# 常用方法
CMake 中，常见的重要的方法，可以保证最终运行成功，或者更加方便得实现所需得功能。

## include_directories
添加目录到包含目录中，基本等同于VS中的 Include Directories。

其中，支持绝对路径和相对路径：

- 绝对路径 对应使用尖括号进行 include
- 相对路径 对应使用双引号进行 include

## target_link_libraries
增加运行链接的库，对应VS中的 Additional Dependencies 配置。

设置需要引入的库名称，经测试，可以不包含扩展名（暂时测试下来 VS 确实可以不带，其他编译器效果不确定）。

## target_link_directories
添加额外的库目录，对应VS中的 Additional Library Directories。

支持相对路径和绝对路径，使用没有区别。

## add_definitions
原本的功能是用于运行时添加定义，可以用于添加运行宏：`-Dxxx` 其中 `xxx` 表示希望添加的宏。

# 常见的宏
- `CMAKE_SOURCE_DIR` 对应VS中的 `SolutionDir`

# 使用参数
CMake 编译生成时通常会使用到的参数。
- `-G "Compiler"` 用于指定某个特定的编译器，如果不进行指定会使用默认的编译器
- `-A Platform` 指定编译后的平台，如果没有指定，一般都是生成 `x64`，可以通过该参数，生成 `Win32` 平台，保证正常运行。

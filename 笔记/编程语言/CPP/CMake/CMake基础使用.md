CMake的基本使用记录，可以配合 Python 脚本的 `cmake-converter` 来进行学习使用。

# 1.常用方法
CMake 中，常见的重要的方法，可以保证最终运行成功，或者更加方便得实现所需得功能。

## 1.1.include_directories
添加目录到包含目录中，基本等同于VS中的 Include Directories。

其中，支持绝对路径和相对路径：

- 绝对路径 对应使用尖括号进行 include
- 相对路径 对应使用双引号进行 include

## 1.2.target_link_libraries

增加运行链接的库，对应VS中的 Additional Dependencies 配置。

设置需要引入的库名称，经测试，可以不包含扩展名（暂时测试下来 VS 确实可以不带，其他编译器效果不确定）。

## 1.3.target_link_directories

添加额外的库目录，对应VS中的 Additional Library Directories。

支持相对路径和绝对路径，使用没有区别。

## 1.4.add_definitions

原本的功能是用于运行时添加定义，可以用于添加运行宏：`-Dxxx` 其中 `xxx` 表示希望添加的宏。

## 1.5.find_package

通过一定的自动规则，查找库的资源。

常见的使用如下：

```cmake
find_package(Vulkan REQUIRED)
target_link_libraries(
	${MODULE_NAME}
		PRIVATE
	Vulkan::Vulkan
)
```

# 常见的宏
- `CMAKE_SOURCE_DIR` 对应VS中的 `SolutionDir`

# 使用参数
CMake 编译生成时通常会使用到的参数。
- `-G "Compiler"` 用于指定某个特定的编译器，如果不进行指定会使用默认的编译器
- `-A Platform` 指定编译后的平台，如果没有指定，一般都是生成 `x64`，可以通过该参数，生成 `Win32` 平台，保证正常运行。

# 常见需求

## 创建Windows Gui项目

希望程序使用 `WinMain` 作为入口，创建GUI项目。

可以通过将 `CMAKE_WIN32_EXECUTABLE` 设置为 `TRUE` 来开启

## 将多个模块放在文件夹（VS的过滤器）下

1. 开启文件夹功能 `set_property(GLOBAL PROPERTY USE_FOLDERS ON)`
2. 在对应的模块下，指定所在模块 `set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "${FOLDER_NAME}")`

如果指定的目录下，存在多层目录结构，希望在 VS 中也保留的话，需要每层文件夹，都使用 `source_group(<文件夹名>)` 进行标记。

```cmake
# 1.设置开启创建文件夹（VS里的筛选器）的功能
# 一般是在根目录的 CMakeLists.txt 里设置全局的开关
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 使用 source_group 设置自动创建文件夹的规则
# - 参数1：文件夹名
# - 参数2：文件名匹配规则，根据每个文件的路径尝试放到最匹配的文件夹下
source_group(Public "Public/")
source_group(Private "Private/")
source_group(Public/Test "Public/Test/")
```

> [!todo] TODO
> 更好的方法探索，目前需要列出所有的文件夹还是比较繁琐，可能在未来的版本里还有更好的写法。

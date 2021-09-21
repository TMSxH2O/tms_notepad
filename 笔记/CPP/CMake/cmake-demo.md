此处，是一个基础的 OpenGL 项目。其中，按照代码的功能，将项目分为四个部分：

1. 引擎<br>项目的核心，需要复用的代码
2. 子模块<br>各个子模块之间分离，互不干扰，在其中实现 OpenGL 中的模块功能
3. 资源<br> res 文件夹，包括图片、着色器以及导出的3D模型资源
4. 外部依赖<br>Dependencies，包括项目必须的第三方代码，如 GLAD、GLFW、GLM等

其中，通过 CMake 管理，需要分为三类 `CMakeLists.txt` 文件：

1. 主体<br>在项目根目录下创建，通过该文件，协调所有的 `CMakeLists.txt` 文件，其中包含了一些全局的变量
2. 引擎<br>项目下的 `src` 文件夹，里面将包含项目共用的框架以及常用的工具类
3. 子模块<br>项目下的 `Examples` 文件夹，其中是很多个独立工作的模块代码，不希望互相干扰，可以独立运作

# 主体
```cmake
# 指定最低版本
cmake_minimum_required(VERSION 3.15.0)

project(demo_opengl_project)

set(PROJECT_NAME "demo_opengl_project")  # 定义项目名常数
set(RES_PATH ${CMAKE_SOURCE_DIR}/res)    # 定义资源路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)  # 设置exe文件输出路径

# 添加全局宏定义
add_definitions(-DGLCORE_PLATFORM_WINDOWS -DGLFW_INCLUDE_NONE -DWIN32 -DRES_PATH="${RES_PATH}")

# 项目includes目录，相对路径使用双引号，绝对路径使用尖括号
include_directories(
	./src
	./src/base
	./src/imgui
	./src/stb_image
	${PROJECTION_SOURCE_DIR}/Dependencies/GLAD/include
	${PROJECTION_SOURCE_DIR}/Dependencies/GLFW/include
	${PROJECTION_SOURCE_DIR}/Dependencies/GLM
	${PROJECTION_SOURCE_DIR}/Dependencies/spdlog/include
	${PROJECTION_SOURCE_DIR}/Dependencies/Assimp
	${PROJECTION_SOURCE_DIR}/src/imgui
)

# subdirection 中都需要有对应的 CMakeLists.txt 文件
add_subdirectory(src)  # 对应引擎 Engine 路径
add_subdirectory(Examples/learnopengl/Bloom)  # 子模块

# 复制dll到最后统一的exe目录
file(GLOB LIB_FILES "*.dll")
file(COPY ${LIB_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
file(COPY ${LIB_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/Released)
```
# 引擎
```cmake
set(TARGET Engine)

file(GLOB_RECURSE SOURCE_SRCS "*.cpp" "*.c")
file(GLOB_RECURSE HEARER_SRCS "*.h")

set(ENGINE_SOURCES ${SOURCE_SRCS} ${HEARER_SRCS})

add_library(${TARGET} STATIC ${ENGINE_SOURCES})

set(ADDITIONAL_LIBRARY_DEPENDENCIES
	${CMAKE_SOURCE_DIR}/Dependencies/spdlog/lib/spdlogd.lib
	${CMAKE_SOURCE_DIR}/Dependencies/Assimp/lib/assimp-vc140-mt.lib
	${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib/glfw3.lib
	opengl32
)
target_link_libraries(${TARGET} PUBLIC ${ADDITIONAL_LIBRARY_DEPENDENCIES})
```
# 子模块
```cmake
set(TARGET Bloom)

file(GLOB_RECURSE BLOOM_SRCS "*.cpp" "*.h")

add_executable(${TARGET} ${BLOOM_SRCS})

target_link_libraries(${TARGET} PRIVATE Engine)
```
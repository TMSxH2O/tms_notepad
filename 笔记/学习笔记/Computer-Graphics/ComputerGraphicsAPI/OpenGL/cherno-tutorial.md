> Cherno 的 OpenGL 教程 [快速跳转](https://www.bilibili.com/video/BV1MJ411u7Bc)
> [OpenGL Docs](https://docs.gl/)

# 概要

OpenGL 作为计算机图形API，是用于运行在显卡（GPU）中的代码框架。此处的学习，其实指的是学习OpenGL的规范，OpenGL 并不代表具体的代码，其类似STL，通过制定某些规范，由显卡供应商来进行实现，其具体的代码将内置在GPU中。

学习使用OpenGL的过程中，所使用到的，如GLFW，其本质的功能是提供了一个指针，用于指向显卡中所实现的方法，通过这样的方式，来调用OpenGL提供的代码。它所做的只是类似，指定OpenGL的规范，提供函数声明、符号声明、常量等，并将其放在某个头文件中，使得之后可以直接调用这些代码。添加的，如`opengl32.lib`的库，其中包含的是系统中实现的代码，之后由编译器将代码进行链接，形成最终的代码。

OpenGL 是一个状态机，可以指定其某个时间运行的状态，而不能获取 OpenGL 对象使其在相同时间完成不同的功能。

# 混合

OpenGL 中，处理颜色混合或者直接理解成颜色叠加

## 基本概念

- `Blending` 混合<br>用于描述如何从当前的颜色(`Target buffer`)得到最终的颜色(`Output`)
- `Output` (`source`)<br>代表从面片着色器中输出的颜色数据
- `Target buffer` (`destination`)<br>代表面片着色器正在绘制的缓冲区（原本的颜色）

通过将原本的颜色，与现在的颜色进行某种方式的混合，得到最终的颜色。

## OpenGL方法

此处将会涉及到的几个最基本的方法

- `glEnable`/`glDisable` 用于启动/关闭混合功能 `GL_BLEND`
- `glBlendFunc`/`glBlendFunci` 指定混合的方法，如何得到原本的颜色和当前颜色
- `glBlendEquation`/`glBlendEquationi` 指定对得到的原本的颜色和当前颜色进行组合的方法(在`glBlendFuc`之后)

### glEnable

当开启了 `GL_BLEND` 之后，会启用渲染缓冲区，存储之前的颜色的具体信息，用于之后的 `glBlendFunc` 进行处理。

### glBlendFunc

混合的重要方法，用于决定混合过程中如何利用缓冲区中的值，来与当前的颜色进行混合。该方法接受一下的参数：

此处得到的颜色值，将会用于 `glBlendEquation` 中。

- `sfactory` 对应 `Output` (`source`)<br>当前输入颜色的处理，默认值为 `GL_ONE`
- `dfactory` 对应 `Target buffer` (`desctination`)<br>颜色缓冲区中得到颜色的处理，默认值为 `GL_ZONE`

`GL_ONE` 和 `GL_ZONE` 的意思，就是在原本的颜色值上乘对应的数字，`GL_ONE` 就是乘1，颜色值不变；`GL_ZONE` 就是乘0，得到的颜色值也就等于0。

此处比较常见的取值主要有如下的值：

- `GL_SRC_ALPHA`<br>使用颜色对应的 `RGBA` 分别乘上 `source` 的透明度($k_a$)
- `GL_ONE_MINUS_SRC_ALPHA`<br>使用颜色对应的 `RGBA` 分别乘上一减 `source` 的透明度($1-k_a$)

### glBlendEquation

对 `glBlendFunc` 的取值进行混合，得到结果。

其中接受以下参数：

- `mode` 混合的模式<br>如何将 `source` 和 `destination` 进行混合。默认取值为 `GL_FUNC_ADD`。

通过指定不同的等式，最终将得到不同的结果。正常情况下，应该保证最终得到的比例依然为1。不然会出现颜色大于1或者小于0的情况。

其中，`mode` 接受以下的值：

- `GL_FUNC_ADD`<br>将两个结果直接相加
- `GL_FUNC_SUBTRACT`<br>使用 `source` 减去 `destination`
- `GL_FUNC_REVERSE_SUBTRACT`<br>使用 `destination` 减去 `source`
- `GL_MIN`<br>取两者中的较小值
- `GL_MAX`<br>取两者中的较大值

> 在接口文档中，也要求此处得到的结果值，应该保证所有的值范围都在 [0, 1]，不然值无效。

# 拓展

## imgui

> https://www.bilibili.com/video/BV1MJ411u7Bc?p=22

仿照其中的样例，可以在 OpenGL 的每次渲染中，添加创建自定义的 GUI 调试窗口。在其中修改数据并实时展示。

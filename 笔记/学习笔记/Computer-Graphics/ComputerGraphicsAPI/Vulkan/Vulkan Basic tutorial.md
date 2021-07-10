# 基础学习

> https://vulkan-tutorial.com/

## 基本流程

Vulkan 的使用流程，与 OpenGL 有部分类似，但是更加复杂。

### 窗口创建

默认情况下 Vulkan 使用的是 GLFW 来创建窗口，因此使用的流程与 OpenGL 类似。

1. 初始化 GLFW

    通过使用 `glfwInit()` 进行初始化，之后通过使用 `glfwWindowHint`  设置窗口的参数。
    
    ```cpp
    glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // 不能修改窗口大小
    ```
    
    设置完成之后，可以创建窗口 `glfwWindow = glfwCreateWindow(width, height, "title", nullptr, nullptr)` 。
    
    创建窗口后需要判断创建是否成功。
    
2. 主循环

    ```cpp
    // 判断窗口是否应该关闭，持续进行循环
    while(!glfwWindowShouldClose(window))
    {
        // 交换事件队列
        glfwPollEvents();
    }
    ```
    
3. 关闭窗口

    ```cpp
    // 销毁窗口
    glfwDestroyWindow(window);
    // 终止 GLFW
    glfwTerminate();
    // 结束程序
    return 0;
    ```

## 小知识笔记

### 交换链

> Vulkan tutorial 中的 Drawing a triangle > Presentation > Swap chain

交换链的意义是控制图像的显示速度，也就是屏幕刷新速度。
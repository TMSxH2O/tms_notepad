使用VSCode编写C++程序所需要的环境，已经产生的笔记。

# 环境搭建

VSCode默认并不支持对特定编程语言的各种支持（此处主要指的是提示、编译以及各种IDE所提供的功能），需要借助大量的插件共同工作来实现所需的功能。

## MinGW

MinGW是将GCC编译器和GNU Binutils移植到Win32平台下的产物。

> 来自维基百科

需要先进行下载并安装（或者找网上的绿色版）。

> https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/

## 插件准备

此处列举将会使用到的VSCode插件。

### C/C++

Microsoft官方提供的C/C++支持的插件，在VSCode插件中直接搜索并下载即可。

#### 配置

1. 通过使用 `Ctrl+Shift+P` 组合键，输入 `C/C++`，选择 `C/C++: Edit Configuration(UI)`
2. 在配置界面中，选择编译器路径，此处选择MinGW的 `bin` 目录下的 `g++.exe` 文件
3. 下方的 `IntelliSense 模式` 选择 `gcc-x64`

如果在左侧的项目目录下出现了 `.vscode` 文件夹，其中包括一个 `c_cpp_properties.json` 文件，则说明上方的配置成功（之后可以直接在json文件中进行修改）

### cpplint

代码风格审查工具，本身只是一个Python脚本，在安装了插件的同时，还需要下载同名Python插件（使用`pip`下载）

```python
# 下载cpplint脚本
pip install cpplint
# 查看下载地址
pip show cpplint
```

之后可以通过在 `setting.json` 文件中添加以下配置即可：

```json
"cpplint.cpplintPath": "cpplint所在文件夹，如：d:\\python3.7\\lib\\site-packages",
```

## 项目配置

在环境准备完成后，已经可以编译（通过控制台命令的方式）并生成文件，以下主要为了使用完整的功能（编译和测试）所需要使用的配置。

### 配置构建任务

1. 通过使用 `Ctrl+Shift+P` 调出命令面板，其中输入 `tasks`，选择 `Tasks: Configure Default Build Task` 创建任务标准配置文件
2. 选择 `C/C++: g++.exe build active file` 会在项目目录下，生成 `.vscode/tasks.json` 文件

```
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        {
            "type": "shell",
            "label": "g++",  // 任务的名字，就是刚才在命令面板中选择的时候所看到的，可以自己设置
            "command": "D:\\Environment\\MinGW\\bin\\g++.exe",
            "args": [  // 编译时候的参数
                "-g",  // 添加gdb调试选项
                "${file}",
                "-o",
                "${workspaceFolder}\\${fileBasenameNoExtension}.exe",  // 此处文件必须和之后的launch.json文件中的program对应，不然会报错
                "-ggdb3",   // 生成和调试有关的信息
                "-Wall",    // 开启额外警告
                "-static-libgcc",   // 静态链接
                "-std=c++17",       // 使用c++17标准
                "-finput-charset=UTF-8",    //输入编译器文本编码 默认为UTF-8
                "-fexec-charset=GB18030",   //输出exe文件的编码
                "-D _USE_MATH_DEFINES"
            ],
            "options": {
                "cwd": "D:\\workspace\\MinGW-w64\\mingw64\\bin"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "presentation": {
                "echo": true,
                "reveal": "always", // 在“终端”中显示编译信息的策略，可以为always，silent，never
                 "focus": false,
                 "panel": "shared" // 不同的文件的编译信息共享一个终端面板
            },
            "group": {
                "kind": "build",
                "isDefault": true  // 表示快捷键Ctrl+Shift+B可以运行该任务
            }
        }
    ]
}
```

### 配置调试设置

1. 直接点击 `F5` 进行调试，会弹出选择菜单，选择 `C++(GDB/LLDB)`
2. 在项目目录下生成默认的配置文件 `.vscode/launch.json`

```
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch", // 配置名称，将会在启动配置的下拉菜单中显示
            "preLaunchTask": "g++", // 调试会话开始前执行的任务，一般为编译程序，c++为g++, c为gcc
            "type": "cppdbg", // 配置类型，这里只能为cppdbg
            "request": "launch", // 请求配置类型，可以为launch（启动）或attach（附加）
            "program": "${workspaceFolder}/${fileBasenameNoExtension}.exe", // 将要进行调试的程序的路径，与tasks.json中的输出文件对应
            "args": [], // 程序调试时传递给程序的命令行参数，一般设为空即可
            "stopAtEntry": false, // 设为true时程序将暂停在程序入口处，一般设置为false
            "cwd": "${workspaceFolder}", // 调试程序时的工作目录，一般为${workspaceFolder}即代码所在目录
            "environment": [],
            "externalConsole": true, // 调试时是否显示控制台窗口，一般设置为true显示控制台
            "MIMode": "gdb",
            "miDebuggerPath": "D:\\workspace\\MinGW-w64\\mingw64\\bin\\gdb.exe", // miDebugger的路径，注意这里要与MinGw的路径对应
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        },
    ]
}
```

## 常见异常

环境搭建阶段遇到的部分问题汇总。

### 出现 program "xxxx.exe" not exits

检查 `launch.json` 中的 `configurations` 下的 `program` 与 `tasks.json` 中的输出 `exe` 文件是否对应

<span style="color:red">**必须保证这两处配置指向同一个文件**</span>

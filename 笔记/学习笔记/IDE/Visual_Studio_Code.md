在 Visual Studio Code 的使用过程中，学到的一些小技巧，能够提高之后的编程的效率。

## 搜索相关

### 正则搜索

在 Visual Studio Code 中的搜索支持正则匹配，需要点击搜索框旁边的 `[.*]` 按钮，就开启了正则匹配。

### 搜索窗口

在搜索界面中，点击快捷键 `Ctrl + Enter` 会新建一个专门的搜索界面，在大窗口中进行显示，将搜索的结果展示在一个文件中。

> 如果需要将搜索匹配的结果进行提取，可以通过以下的操作对结果进行提取（在正则搜索中可能会经常使用）：
> 
> 使用快捷键 `Ctrl + Shift + l` 选择其中的所有的匹配结果，之后点击 `Ctrl + c` 就能实现结果的批量复制

## 常见快捷键

```bash
Alt + Shift + F     # 文件格式化（需指定对应对文件分析）
Ctrl + Shift + p    # 开启命令面板
Ctrl + `            # 开启调用系统的命令窗口 Terminal 窗口
```

## Python

在 Visual Studio Code 中编写 Python 的代码，以及其插件的相关使用。

### Debug

对当前文件进行 `Debug` 操作，点击 `F5` ，选择其中的 `Python File`

之后的操作方法与其他很多的软件类似，不过多赘述

### 快速开启 Python 解释器

Python 自带交互式的解释器，本来在计算机的命令窗口中（如果已经设置了环境变量），通过使用 `python` 就会进入解释器。

在解释器中，会逐行接受并解释代码。

在 VSCode 的命令窗口中，输入 `Python: Start REPL` 来快速进入 Python 解释器。
# PyQt5

> PyQt5 基础的部分的学习，主要资料来自 [三码农](https://www.sanmanong.com/pyqt5_intro.html)

## 基础结构

运行 PyQt5 的基础结构，只需要需要一个 `QApplication` 组件才能运行，在其中，添加 `QWidget` 先包含具体的窗口中的小部件，因此，最基础的逻辑如下。

```python
import sys
from PyQt5.QtWidgets import QApplication, QWidget, ..

class Demo(QWidget):
    
    def __init__(self):
        super(QWidget, self).__init__()
        ...

if __name__ == '__main__':
    app = QApplication(sys.argv)    # 必须包含一个 `QApplication`
    demo = Demo()                   # 创建一个在应用中显示的部件
    demo.show()                     # 默认为隐藏，需要调用 show() 函数来设置其显示
    sys.exit(app.exec())            # 循环执行 app.exec() 并最终 app.exec() 的返回值传给 sys.exit() 进行退出
```

## 核心内容

### 信号和槽

在 Qt 中的重要概念，信号（`Signal`）和槽（`Slot`），使用进行组件之间的通信。

在组件的某个事件发生时，就会发送某种信号，通过为其连接上某个槽，就能通过具体的函数来进行组件之间的调用，或交互。

```python
# 此处存在一个按钮 QPushButton
self.btn_signal.clicked.connect(self.ChangeInfo)  
# 1. 为按钮的点击clicked时间绑定一个信号
# 2. 通过使用 connect() 设置连接到某个槽（某个函数）
# 3. 在按钮触发点击时间是，就会发出信号来调用槽

...

def ChangeInfo(self):   # 在按钮被点击时进行调用
    pass
```

通过在不同的事件上绑定信号，从而在某个具体的时间点来调用函数，进行某些操作或数据变换。

#### 注意事项

1. `connect()` 接收的是一个函数对象，而不是函数调用，因此一般只需要传入函数名
2. 如果传入 `connect()` 的函数<u>需要接收某个参数</u>，需要借助 `lambda` 来指定具体的函数调用方式

### 窗口大小相关

在 Qt 中，将窗口主要分为两个部分，标题栏和窗口主体。同时，其中提供了两类的函数，分别用于获取长度和坐标。

> Qt 中的坐标都是在原点在左上角，横向从左往右 x 轴增长，纵向从上到下 y 轴增长

+ `x()` 获取窗口整体最左上角，相对于显示器的 x 轴坐标
+ `y()` 获取窗口整体最左上角，相对于显示器的 y 轴坐标
+ `pos()` 返回窗口整体最左上角的坐标 `PyQt5.QtCore.QPoint` 类型
+ `width()` 返回窗口主体的宽度
+ `height()` 返回窗口主体的高度
+ `geometry()` 返回象征窗口主体的矩阵，类型为 `PyQt5.QtCore.QRect`

    其中包含部分得到窗口主体的更多信息的函数：
    
    + `x()` 矩阵的左上角 x 坐标
    + `y()` 矩阵的左上角 y 坐标
    + `width()` 矩阵的长
    + `height()` 矩阵的宽

+ `frameGeometry()` 得到象征整个窗口的矩阵（与 `geometry()` 效果类似）

> 官方的 Qt 窗口结构图
>
> ![结构图](https://www.sanmanong.com/wp-content/uploads/2020/07/1593923132-14c880ee7a70043.jpg)
>
> 其中，将窗口分成了多个矩阵，不同的矩阵来实现不同的功能，本质上与早期的 HTML 进行分块效果类似。
>
> <u>其中的矩形与矩形或边界之间，都存在间隙，默认为 `1px`。</u>标题栏默认的高度为 `36px`（可能不准确）。

### 事件处理

事件是由窗口、控件本身或者PyQt5自身产生的，配合用户动作的各种响应。事件和信号非常相似，但请不要归为一类。事件类型种类繁多，常用的事件处理有：

1. 窗口关闭事件
2. 鼠标事件
3. 键盘事件

#### 窗口关闭事件

窗口关闭事件通过重写其中的 `closeEvent(QCloseEvent)` 函数实现自定义的功能。

`QCloseEvent` 表示窗口是否退出的事件，其中可以用于控制是否进行退出。

+ `accept()` 退出
+ `ignore()` 忽略

#### 鼠标事件

主要通过重写其中的几个函数来实现对不同事件的监控：

+ `mouseMoveEvent(QMouseEvent)`
+ `mousePressEvent(QMouseEvent)`
+ `mouseReleaseEvent(QMouseEvent)`
+ `mouseDoubleClickEvent(QMouseEvent)`

在 `QMouseEvent` 中可以得到鼠标的相关信息：

+ `x()`
+ `y()`
+ `globalX()` 全局坐标（相对于屏幕）
+ `globalY()`
+ `button` 得到点击的按键，可以在 `QtCore.Qt` 中得到对应的值

#### 键盘事件

通过重写以下函数来实现对不同事件的监控：

+ `keyPressEvent(QKeyEvent)`
+ `keyReleaseEvent(QKeyEvent)`

在 `QKeyEvent` 中可以得到具体的键盘信息：

+ `key()` 得到按键的信息，在 `QtCore.Qt` 中得到对应的值

## 拓展

可能会用到的比较零碎的内容

### 定时器

定时器其中包括两个核心的函数：

+ `start(:ms int:)` 开启定时器，其中指定定时器的时间，在指定时间之后会自动结束
+ `stop()` 手动停止某个定时器

在其中，存在比较重要的 Signal：

+  `timeout` 在定时器运行结束时触发

### 消息对话框

在 Qt 中支持多种不同的对话框，其中，常见的有：

+ question
+ information
+ warning
+ critical
+ about

一般的对话框都存在四个属性，分别表示：

1. 父模块
2. 对话框标题
3. 对话框信息
4. 按钮（多个按钮之间使用 `|` 分割）【有部分没有】

常见的按钮有：

+ `QMessageBox.Ok`
+ `QMessageBox.Yes`
+ `QMessageBox.No`
+ `QMessageBox.Cancel`
+ `QMessageBox.Close`
+ `QMessage.Open`
+ `QMessage.Save`

信号对话框的函数最终会返回所点击的按钮，因此只需要在之后判断点击不同的按钮进行对应的处理。（如下实例）

```python
# 确认是否需要进行保存
choice = QMessageBox.question(self, 'Exit', 'Do you want to save the text?',
                                          QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)
                                          
# 注意，此处必须使用 == 进行判断，is得到的不是同一个值，虽然很像是单例模式，但实际不是
if choice == QMessage.Yes:  # yes
    ...
elif choice == QMessage.No: # no
    ...
else:                       # cancel
    ...
```

### 颜色选择框

通过使用 `QColorDialog.getColor()` 来开启颜色选择框，并返回一个 16 禁止的对应颜色的值。

### 输入对话框

弹出一个带有输入栏的对话框，在其中提示并输入信息。

通过使用 `QInputDialog.getText([父元素], [标题], [提示信息])` 来得到一个元组，其中包含输入的数据以及是否确定输入（在窗口中会出现 ok 和 cancel 按钮，只有点击 ok 返回 True；cancel 反之）。

### 文件对话框

根据文件对话框所选取的文件，得到其文件的路径信息。

`QFileDialog.getOpenFileName([父元素], [标题], [文件过滤]) -> tuple([文件路径], [文件过滤])` 

其中文件过滤是一个特殊格式的字符串：

> The file dialog's working directory will be set to dir. If dir includes a file name, the file will be selected. Only files that match the given filter are shown. The filter selected is set to selectedFilter. The parameters dir, selectedFilter, and filter may be empty strings. If you want multiple filters, separate them with `;;`, for example:
>
>  `"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"`

即按照 `[自定义命名] ([过滤方式]) ;; ...` 的方式，指定文件过滤。

### 剪贴板

通过调用 `QApplication.clipboard()` 来初始化剪贴板，得到一个 `QtGui.QClipboard` 对象。

其中同样支持 `setText()` `mimeData()` 等函数来实现数据的交互。

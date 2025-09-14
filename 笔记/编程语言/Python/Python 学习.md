# Python

在 `Python` 学习的过程中，所产生的的笔记内容。

> 学习路径:
>
> [Github] [Python 100 Days](https://github.com/jackfrued/Python-100-Days/blob/master/Day01-15/02.%E8%AF%AD%E8%A8%80%E5%85%83%E7%B4%A0.md)

## 阶段划分

在文档中，将整个学习的过程，分为了12个阶段，难度由浅入深。

1. Python 语言基础 （1~15）
2. Python 语言进阶 （16~20）
3. Web 前端入门 （21~30）
4. 玩转 Linux 操作系统 （31~35）
5. 数据库基础和进阶 （36~40）
6. 实战 `Django` （41~55）
7. 用 FastAPI 开发数据接口 （56~60）
8. 爬虫开发 （61~65）
9. 数据分析 （66~70）
10. 机器学习和深度学习 （71~85）
11. 大数据分析概述 （86~90）
12. 团队项目开发 （91~100）

> 目前学习内容繁多，将只学习到 Python 语言进阶部分，之后的内容如有需要，会部分学习

## 面向对象进阶

在了解了面向对象的基础上，进行拓展对面向对象编程进行更深入的了解。

### @property 装饰器

> 在 Python 中，虽然可以通过命名时在名称前添加下划线的方式，来实现 `private` 访问修饰。但是这样的方式并不推荐，因为在这样的变量或函数无法被子类继承，容易出现问题，同时在 Python 中提供了更加优秀的解决方式，也就是此处的装饰器。

为了实现面向对象中最基本的封装，可以对类中的属性命名时添加一个下划线，使其变成 `protected` 子类中也可以直接访问对象的属性，但是在类外不能直接访问，而是需要借助 `getter` 和 `setter` 函数来实现间接的操作。

在 Python 中，定义 `getter` 和 `setter` 函数的方式如下：

```python
class Person(object):

    def __init__(self, name, age):
        self._name = name
        self._age = age

    # 访问器 - getter方法
    @property
    def name(self):
        return self._name

    # 访问器 - getter方法
    @property
    def age(self):
        return self._age

    # 修改器 - setter方法
    @age.setter
    def age(self, age):
        self._age = age

    def play(self):
        if self._age <= 16:
            print('%s正在玩飞行棋.' % self._name)
        else:
            print('%s正在玩斗地主.' % self._name)


def main():
    person = Person('王大锤', 12)
    person.play()
    person.age = 22
    person.play()
    # person.name = '白元芳'  # AttributeError: can't set attribute
```

> **总结**
>
> 通过使用 `@property` 来定义 `getter` 函数，而使用 `@<属性名>.setter` 来指定 `setter` 函数

### \_\_slots__

Python 的一大特点就是其动态性，类在定义了之后，其中的属性事实上还可以通过特殊的方式进行增加或删除，导致并不能确定其具体的成员属性（或者每次都访问前都使用 `hasattr()` 函数进行确认）

为了解决这个问题，其中提供了 `__slots__` 属性，能够锁定其中的成员属性，其接受一个元组，元组中定义的都是成员属性的名称。之后就无法在外部直接强行为对象添加属性了。

```python
class Demo:
    # 如果使用了以下代码，之后非法的赋值操作就会报错
    # __slots__ = ()  直接锁定该类不接受任何的成员属性
    # __slots__ = ('m_bTest')  该类中只接受名称为 m_bTest 的成员属性
    pass

def main():
    demo = Demo()
    demo.m_bTest = True
    print(demo.m_bTest) # 成功输出 `True`
```

### 抽象类和抽象方法

在定义一个抽象类时，需要引入 `abc` 中的 `ABCMeta` 和 `abstractmethod`。之后，通过在方法上添加 `@abstractmethod` 注释，表示该方法为抽象方法。

```python
class AbstratDemo(object, metaclass=ABCMeta):
    ...
    @abstractclass
    def foo(self):
        pass
    ...
```

### 静态方法和类方法

> 实际的使用场景不多，此处不多介绍

在类中的方法上添加 `@staticmethod` 表示静态方法；而添加 `@classmethod` 表示类方法

## 异常相关

在 Python 中的异常处理相关内容。

> 参考
>
> [Python 中的异常处理](https://segmentfault.com/a/1190000007736783)

### 异常基础使用

在 Python 中的异常，使用的是 `try..except..finally..`，分成三个部分。（基本逻辑与 Java、CPP 一致）

其中主要需要注意以下几点内容：

1. `except` 语句不是必须的，`finally` 语句也不是必须的，但是二者必须要有一个，否则就没有 `try` 的意义了。
2. `except` 语句可以有多个，Python会按 `except` 语句的顺序依次匹配你指定的异常，如果异常已经处理就不会再进入后面的 `except` 语句。
3. `except` 语句可以以元组形式同时指定多个异常，参见实例代码。
4. `except` 语句后面如果不指定异常类型，则默认捕获所有异常，你可以通过 `logging` 或者 `sys` 模块获取当前异常。
5. 如果要捕获异常后要重复抛出，请使用 `raise`，后面不要带任何参数或信息。
6. 不建议捕获并抛出同一个异常，请考虑重构你的代码。
7. 不建议在不清楚逻辑的情况下捕获所有异常，有可能你隐藏了很严重的问题。
8. 尽量使用内置的异常处理语句来替换 `try/except` 语句，比如 `with` 语句，`getattr()` 方法。

### 嵌套异常

如果在代码中出现了嵌套的异常（该函数所包含的函数中存在异常，并且并不希望在当前函数中解决，而希望向更上层抛出错误）。

此处主要分为两种情况：

1. 完全没有考虑过在当前函数中处理异常

    这是比较理想的情况，将问题向上抛出，在某个阶段统一处理，那么就不需要对当前的函数进行任何的处理，即函数中出现了问题没有被捕获就会自动向更上层抛出，直到被 `except` 或 Python 解释器捕获。
    
2. 无法避免得捕获到了某个异常，但是要对这个异常进行放行

    在捕获到了该异常之后，直接使用 `raise` 放行（不是将异常对象进行上抛）
    ```python
    def foo():
        ...
        try:
            function() # 假定其中会出现某个异常，要对这个异常向上放行
        except Exception as e:
            raise   # 直接使用 raise 抛出异常，而是 raise e 这样的方式，不然会导致其中的信息丢失
    ```

### 使用内置的语法代替 `try/except`

在 Python 中提供了很多语法来简化异常的处理，能简化异常的处理，避免出错。

+ `for` 语句中就处理了 `StopIteration` 异常，可以很方便得实现循环遍历
+ `while` 语句来处理文件开启会在最后自动对其进行关闭操作，可以更加稳定得实现文件的读取操作
+ `hasattr()` 可以判断对象中是否存在某个函数，如果不存在，还能返回设置的默认值

### 最佳实践

最佳实践不限于编程语言，只是一些规则和填坑后的收获。

1. 只处理你知道的异常，避免捕获所有异常然后吞掉它们。
2. 抛出的异常应该说明原因，有时候你知道异常类型也猜不出所以然。
3. 避免在catch语句块中干一些没意义的事情，捕获异常也是需要成本的。
4. 不要使用异常来控制流程，那样你的程序会无比难懂和难维护。
5. 如果有需要，切记使用finally来释放资源。
6. 如果有需要，请不要忘记在处理异常后做清理工作或者回滚操作。

## 进程与线程

在 Python 中的多线程，主要可以被分为以下三种：
+ 多进程
+ 多线程
+ 多进程 + 多线程

### 多进程

此处的逻辑会因为系统的不同而有所不同。

+ 在 Linux 和 Unix 中，提供了 `fork()` 系统调用来创建进程，调用 `fork()` 的就是父进程，创建出的进程就是子进程。子进程就是父进程的拷贝，但是子进程拥有自己的 PID。
    > `fork()` 函数很特殊，会进行两次返回，分别是调用函数，在父进程中的返回值是子进程的 PID，同时，子进程会有自己的返回值（但是固定是0）。
    > 
    > Python 的 os 模块中，提供了 `fork()` 函数。
+ 在 Windows 中，没有对应的 `fork()` 系统函数，可以通过使用 multiprocessing 模块的 `process` 类来创建子进程。
    > 同时，该模块中还提供了更多高级的实现，如批量启动进程的进程池（`Pool`）、用于进程间通信的队列（`Queue`）和管道（`Pipe`）等。

实例代码如下，感受使用多进程代码的效果：
```python
# 没有使用多进程的代码
from random import randint
from time import time, sleep


def download_task(filename):
    print('开始下载%s...' % filename)
    time_to_download = randint(5, 10)
    sleep(time_to_download)
    print('%s下载完成! 耗费了%d秒' % (filename, time_to_download))


def main():
    start = time()
    download_task('Python从入门到住院.pdf')
    download_task('Peking Hot.avi')
    end = time()
    print('总共耗费了%.2f秒.' % (end - start))


if __name__ == '__main__':
    main()
    
# 使用了多进程的代码
from multiprocessing import Process
from os import getpid
from random import randint
from time import time, sleep


def download_task(filename):
    print('启动下载进程，进程号[%d].' % getpid())
    print('开始下载%s...' % filename)
    time_to_download = randint(5, 10)
    sleep(time_to_download)
    print('%s下载完成! 耗费了%d秒' % (filename, time_to_download))


def main():
    start = time()
    p1 = Process(target=download_task, args=('Python从入门到住院.pdf', ))
    p1.start()
    p2 = Process(target=download_task, args=('Peking Hot.avi', ))
    p2.start()
    p1.join()
    p2.join()
    end = time()
    print('总共耗费了%.2f秒.' % (end - start))


if __name__ == '__main__':
    main()
```

其中需要额外注意的是，在多进程代码中，使用 `start()` 函数开启了子进程，之后又<u>使用了 `join()` 函数，让父进程等待所有子进程的结果</u>（因为多进程的执行是不会等待的，简单理解，会等到最后一个进程执行完毕）

### 多线程

在 Python 中提供了多种不同的多线程模块，推荐使用 `threading` 模块（`_thread` 模块过于底层，很多功能的没有提供，一般不推荐使用）

> **为什么不推荐使用 thread 模块**
>
> 最普遍的（网上较多的说法）是 `thread` 中比较低级，即其中缺少很多重要的实现。此外，还有几个比较重要的原因。
>
> + `thread` 存在一个更加完善的上级替代品 `threading`（功能相同），且其中的很多函数与其重名，导致可能发生冲突。
> + `thread` 模块中的同步原语很少（实际上只有一个），而 `threading` 中就很多。
> + `thread` 所提供的线程在退出时没有进行任何的控制，当主线程结束时就直接强制结束。`threading` 中的实现至少会在主线程退出之前确保子线程已经全部退出。
>
> 因此，一般只推荐研究多线程的专家来使用 `thread` 更加原始更加底层。
> 
> <u>在 Python3 中，该模块被改名为 `_thread`。</u>

```python
from random import randint
from threading import Thread
from time import time, sleep


def download(filename):
    print('开始下载%s...' % filename)
    time_to_download = randint(5, 10)
    sleep(time_to_download)
    print('%s下载完成! 耗费了%d秒' % (filename, time_to_download))


def main():
    start = time()
    t1 = Thread(target=download, args=('Python从入门到住院.pdf',))
    t1.start()
    t2 = Thread(target=download, args=('Peking Hot.avi',))
    t2.start()
    t1.join()
    t2.join()
    end = time()
    print('总共耗费了%.3f秒' % (end - start))


if __name__ == '__main__':
    main()
```

还可以使用继承的方式，继承 `threading` 中的 `Thread` 类，并实现自己的 `run()` 函数。（与 Java 中的使用基本一致，此处不多赘述）

```python
from random import randint
from threading import Thread
from time import time, sleep


class DownloadTask(Thread):

    def __init__(self, filename):
        super().__init__()
        self._filename = filename

    def run(self):
        print('开始下载%s...' % self._filename)
        time_to_download = randint(5, 10)
        sleep(time_to_download)
        print('%s下载完成! 耗费了%d秒' % (self._filename, time_to_download))


def main():
    start = time()
    t1 = DownloadTask('Python从入门到住院.pdf')
    t1.start()
    t2 = DownloadTask('Peking Hot.avi')
    t2.start()
    t1.join()
    t2.join()
    end = time()
    print('总共耗费了%.2f秒.' % (end - start))


if __name__ == '__main__':
    main()
```

## 拓展知识

### yield 关键字

在 Python 中，存在一个特殊的关键字 —— `yield`

其具体的作用很特殊，在概念中说明，使用了 `yield` 关键字的函数类似使用了 `return` 关键字，但是返回的是一个迭代器。

参考以下的代码：
```python
def foo():
    print("starting...")
    while True:
        res = yield 4
        print("res:",res)
g = foo()
print(next(g))
print("*"*20)
print(next(g))
```
代码的输出如下：
```bash
starting...
4
********************
res: None
4
```
可以看出，其中 `foo()` 函数的返回值事实上是一个迭代器，可以使用 `next()` 函数向后遍历。而其中存在一个 `res` 返回值为 `None`，这事实上是迭代器中每次的结束，就是下一次的开始，而使用 `next()` 函数导致下一次的循环中，没有接受任何的输入值，因此此处的赋值 `res = None`。

使用 `send()` 函数之后：
```python
def foo():
    print("starting...")
    while True:
        res = yield 4
        print("res:",res)
g = foo()
print(next(g))
print("*"*20)
print(g.send(7))
```
代码输出为：
```bash
starting...
4
********************
res: 7
4
```

### 函数装饰器

> 参考 by 菜鸟教程
>
> [Python 函数装饰器](https://www.runoob.com/w3cnote/python-func-decorators.html)

在 Python 中，函数可以被看作是一个对象，因此出现了在函数中<u>嵌套定义</u>函数的情况，最终返回的是内部的函数对象，这样的方式，就被称为函数装饰器。（甚至有一个专门的设计模式叫装饰器模式，就可以通过 Python 的装饰器轻松完成，事实上，使用的过程类似 Java 中**动态代理**，能够很轻松的实现在某个方法的基础上进行拓展的功能）

最基础的示例代码如下：

```python
def a_new_decorator(a_func):
 
    def wrapTheFunction():
        print("I am doing some boring work before executing a_func()")
 
        a_func()
 
        print("I am doing some boring work after executing a_func()")
 
    return wrapTheFunction
 
def a_function_requiring_decoration():
    print("I am the function which needs some decoration to remove my foul smell")
 
a_function_requiring_decoration()
#outputs: "I am the function which needs some decoration to remove my foul smell"
 
a_function_requiring_decoration = a_new_decorator(a_function_requiring_decoration)
#now a_function_requiring_decoration is wrapped by wrapTheFunction()
 
a_function_requiring_decoration()
#outputs:I am doing some boring work before executing a_func()
#        I am the function which needs some decoration to remove my foul smell
#        I am doing some boring work after executing a_func()
```

在上方的例子中，向函数中输入了另一个函数对象，最终的效果是<u>在没有破坏原代码的基础上，又对其功能进行了增强</u>，这样的效果十分类似 Java 中的动态代理，但是实现更加简单方便。

此外，Python 中还支持一种再在此基础上进行简化的语法糖，使用注解的方式，直接快速定义一个函数装饰器。实现代码如下：

```python
@a_new_decorator
def a_function_requiring_decoration():
    """Hey you! Decorate me!"""
    print("I am the function which needs some decoration to "
          "remove my foul smell")
 
a_function_requiring_decoration()
#outputs: I am doing some boring work before executing a_func()
#         I am the function which needs some decoration to remove my foul smell
#         I am doing some boring work after executing a_func()
 
#the @a_new_decorator is just a short way of saying:
a_function_requiring_decoration = a_new_decorator(a_function_requiring_decoration)
```

> 上方的代码中，存在一个问题:
> ```python
>   print(a_function_requiring_decoration.__name__)
> ```
> 上方函数的输出是 `wrapTheFunction`，说明在 `a_new_decorator` 虽然能够返回函数的对象，但是有对函数中的信息进行了覆盖，包括函数名以及函数文档。
> 
> 为了解决这个问题，可以使用 Python 所提供的工具 `functools.wraps`，它能够方便得复制函数中的基本信息，被注解的函数中。具体使用改进如下

```python
from functools import wraps
 
def a_new_decorator(a_func):
    @wraps(a_func)
    def wrapTheFunction():
        print("I am doing some boring work before executing a_func()")
        a_func()
        print("I am doing some boring work after executing a_func()")
    return wrapTheFunction
    
@a_new_decorator
def a_function_requiring_decoration():
    """Hey yo! Decorate me!"""
    print("I am the function which needs some decoration to "
          "remove my foul smell")
 
print(a_function_requiring_decoration.__name__)
# Output: a_function_requiring_decoration
```

通过以上的例子，可以发现，在 Python 中，注解本质上也是函数，注解会调用该函数，并对被注解的函数进行替换。

通过这样的性质，可以实现动态规划、函数接口替换（注册抽象函数，最后在将注册其实现函数，在抽象接口的检测过程中发现了其实现函数，就返回其实现的函数）。

### 正则相关

> 主要参考
> + [python 100 days - 字符串和正则表达式](https://github.com/jackfrued/Python-100-Days/blob/master/Day01-15/12.%E5%AD%97%E7%AC%A6%E4%B8%B2%E5%92%8C%E6%AD%A3%E5%88%99%E8%A1%A8%E8%BE%BE%E5%BC%8F.md)
> + [正则表达式30分钟入门教程](https://deerchao.cn/tutorials/regex/regex.htm)

> *等待之后完成*

### Python Debug

在 Python 中的 debug 相关模块以及涉及到的常用函数包括指令。

#### logging

通过使用 Python 中内置的 logging 模块，对很多代码的关键部分进行输出打印（包括打印到控制台以及输出到文件）

基础的使用方式如下：

```python
import logging
# 之后的日志重要程度从上到下依次增加
logging.debeg('debug message')
logging.info('info message')
logging.warn('warn message')
logging.error('error message')
logging.critical('critical message')
```

但是，logging 的输出存在一个默认的级别，默认情况下，其中的输出级别为 `warn`，即只会输出 `warn` 级别以及更重要的日志。可以通过调整其中的输出级别的方式，使其打印出更多的信息。

##### 常用函数

+ `logging.basicConfig()` `param :filename string:, :level:`

    其中，如果指定了 `filename` 参数，就会使其修改默认的输出到文件中（默认不会有控制台打印）；`level` 用于指定输出的日志级别，使用类中所指定的常量

+ `logging.Formatter()` `param :fmt string:`

    根据所指定的字符串，生成日志的格式对象。
    
    其中可以指定 `asctime`、`pathname`、`lineno`、`levelname`、`message`（也可以省略某些值），通过使用如 `%(asctime)` 这样的形式来确定具体的输出格式。
    
+ `logging.StreamHandler()` `param :IO:`
    
    获取日志的流处理器，如果没有指定其中的 IO，会选择默认的输出，即控制台

+ `logging.handlers.TimedRotatingFileHandler()` `param :filename StrPath:, :when string:, :interval int:, :backupCount int:, :encoding string:`

    间隔指定时间进行循环的处理器。
    
    `interval` 处理间隔时间；`backupCount` 最大备份数量；`encoding` 编码
    
    `when` 决定单位：`S` 秒、`M` 分、`H` 小时、`D` 天、`W` 星期
    
+ `logging.getLogger()` `param :name string:`

    获取日志对象
    
+ `logger.addHandler()` `param :handler logging.handlers:`

    为某个日志对象添加处理器

+ `logging.setLevel()` `param :level:`

    设置日志的输出级别
    
##### 使用实例

默认情况下，日志的打印会显示在控制台，如果通过使用 `basicConfig()` 指定了 `filename` 就会将日志输出到文件中，控制台就不会有任何输出。

**==如何实现同时打印到控制台又输出到文件？==**

```python
import logging
import logging.handlers

LOGGING_FILE = '...'    # 指定日志输出的文件
LOGGING_LEVEL = logging.INFO    # 此处以 INFO 为例

logger = logging.getLogger(LOGGING_FILE)    # 获取一个日志对象，其与一个文件进行绑定
log_format = logging.Formatter('%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s')   # 指定日志的显示格式
logger.setLevel(LOGGING_LEVEL)  # 设置日志的显示级别
console_stream = logging.StreamHandler()    # 获取能够输出到控制台的处理器
console_stream.setFormatter(log_format)     # 设置打印到控制台的格式
file_stream = logging.handlers.TimedRotatingFileHandler(filename=LOGGING_FILE, when='D', backupCount=3, encoding='utf-8')    # 获取一个文件输出的处理器
file_stream.setFormatter(log_format)
logger.addHandler(console_stream)       # 为该日志对象添加能够打印到控制台的功能
logger.addHandler(file_stream)          # 为该日志对象添加能够输出到文件中

# 具体的使用，就是调用某个具体的 logger 对象
logger.debug('debug message')
logger.info('info message')
...
```

#### pdb

Python 中提供了一个工具，用于实现指令界面的代码调试。

在运行时，通过 `python -m pdb [文件]` 来开启，就会进入 pdb 调试的界面，其中包括如下的指令。

+ `b` 设置断点
+ `c` 继续执行
+ `l` 查看当前代码片段
+ `s` 进入函数
+ `r` 执行代码直到从当前函数中返回
+ `n` 执行下一行
+ `pp` 打印变量的值
+ `help` 帮助界面
+ `q` 终止并退出

这样的方式设置断点可能会显得比较困难，可以直接在代码中，通过使用 `pdb.set_trace()` 函数，直接在代码中设置断点。

### 模块记录

在此处记录部分功能看上去可能会经常使用到的模块。

#### itertools

迭代的相关工具，能够对任何支持迭代的对象进行操作，使其返回一个对其处理后的迭代器。

+ `permutations(:iter:)`
    
    传入一个可迭代对象，返回其全排列迭代器
+ `combinations(:iter:, :int:)`

    传入一个迭代器和限定长度，返回其指定长度的组合
+ `product(:iter:, :iter:)` 

    两个迭代器的中各项的全组合
+ `cycle(:iter:)`

    产生该迭代器的无限循环序列（序列末尾连接序列首）
    
### heapq

堆排序，获取其中最大/小的n个元素

+ `nlargest(:int:, :iter:[, key=:func:])`
    
    返回前n个最大的元素
+ `nsmallest(:int:, :iter:[, key=:func:])`

    返回前n个最小的元素
    
### collections

常用的工具类：

+ `namedtuple`：命令元组，它是一个类工厂，接受类型的名称和属性列表来创建一个类。
+ `deque`：双端队列，是列表的替代实现。Python中的列表底层是基于数组来实现的，而deque底层是双向链表，因此当你需要在头尾添加和删除元素是，deque会表现出更好的性能，渐近时间复杂度为 O(1)。
+ `Counter`：dict的子类，键是元素，值是元素的计数，它的most_common()方法可以帮助我们获取出现频率最高的元素。Counter和dict的继承关系我认为是值得商榷的，按照CARP原则，Counter跟dict的关系应该设计为关联关系更为合理。
+ `OrderedDict`：dict的子类，它记录了键值对插入的顺序，看起来既有字典的行为，也有链表的行为。
+ `defaultdict`：类似于字典类型，但是可以通过默认的工厂函数来获得键对应的默认值，相比字典中的setdefault()方法，这种做法更加高效。

> 转自
>
> [Python语言进阶](https://github.com/jackfrued/Python-100-Days/blob/master/Day16-20/16-20.Python%E8%AF%AD%E8%A8%80%E8%BF%9B%E9%98%B6.md)

#### threading

Python 中内置的多线程模块，其中存在部分的比较常用的函数，如可以实现简单的自定义定时器(`threading.Timer`)。

##### Timer

定时器类，能够传入结束的时间，以及调用的函数，最终通过使用 `start()` 函数进行使用。以下为使用的实例：

```python
import threading
import time

def MyTimer():
    global start_time
    global timer
    global count_timer
    print('[{count}] timer at {time}s'.format(count=count_timer, time=int(time.time()-start_time)))
    count_timer += 1
    timer = threading.Timer(2.0, MyTimer)
    timer.start()

start_time = time.time()
count_timer = 0
timer = threading.Timer(1, MyTimer)
timer.start()

time.sleep(10)
timer.cancel()
```

> 最开始设定在 1 秒之后调用定时器，在定时器绑定的函数中，指定了每 2 秒进行一次循环调用。
>
> 最终在主线程中，休眠 10 秒，之后关闭定时器。


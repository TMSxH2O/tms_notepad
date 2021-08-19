# Python 核心编程

> 该文档是读 《Python 核心编程》（第3版）过程中所产生的笔记。
> 
> 在该书中，是通过一系列小的知识点的方式，来进行介绍，因此可能会显得比较零碎。

## 通用应用

### 正则表达式

Python 中的正则表达式与其他的正则表达式基本通用。其底层使用的是 `re` 模块来进行实现，换句话说，如果需要使用 Python 的正则表达式，需要引入 `re` 模块。

#### 进阶正则表达式

> 之前选择忽略的正则表达式，现在进行补充

+ `(?:\w+\.)*`

    以句点作为结尾的字符串，例如“google.”、“twitter.”，但是这些匹配不会保存下来供之后的使用和数据检索（`group()` 函数无法获取）

+ `(?#comment)`

    将会忽略其中的内容，只是注释。
    
+ `(?=.com)`

    如果一个字符串后跟着 “.com” 才做匹配操作，并不适用任何目标字符串
    
+ `(?!.net)`

    如果一个字符串后面不是跟着 “.net” 才做匹配操作（与 `(?=)` 相对）
    
+ `(?<=800-)`

    如果一个字符串之前为 “800-” 才做匹配，假定为电话号码你，同样，并不适用任何输入字符串（与 `(?=)` 相对）
    
+ `(?<!192\.168\.)`

    如果一个字符串之前不是 “192.168.” 才做匹配操作（与 `(?<=)` 相对）

+ `(?(1)y|x)`

    如果一个匹配组1（\1） 存在，就与 y 匹配；否则，就与 x 匹配（三元运算符）
    
+ `(?P<[name]>)` <=> `(?P=[name])`/`\g<[name]>`

    需要组合使用，左侧的表达式本质上是将匹配到的值通过别名进行存储，右侧的表达式本质上就是通过名称来调用对应的子项。
    
    > 实例
    > 
    > ```python
    > >>> bool(re.match(r'\((?P<areacode>\d{3})\) (?P<prefix>\d{3})- 
    >       (?P<number>\d{4}) (?P=areacode)-(?P=prefix)-(?P=number)
    >       1(?P=areacode)(?P=prefix)(?P=number)',
    >       '(800) 555-1212 800-555-1212 18005551212'))
    > True
    > ```

#### 常用函数

+ `compile(pattern, flags=0)`

    将字符串根据所制定的 flags（标记） 来编译成正则表达式对象
    
+ `match(pattern, string, flags=0)`

    尝试使用带有可选的标记的正则表达式的模式来匹配字符串。如果匹配成功，就返回匹配对象；如果失败，就返回 None

+ `search(pattern, string, flags=0)`

    使用可选标记搜索字符串中第一次出现的正则表达式模式。如果匹配成功，则返回匹配对象；如果失败，则返回 None
    
+ `findall(pattern, string[, flags])`

    查找字符串中所有（非重复）出现的正则表达式模式，并返回一个匹配列表
    
+ `finditer(pattern, string[, flags])`

    与 findall()函数相同，但返回的不是一个列表，而是一个迭代器。对于每一次匹配，迭代器都返回一个匹配对象

+ `split(pattern, string, max=0)`

    根据正则表达式的模式分隔符，`split` 函数将字符串分割为列表，然后返回成功匹配的列表，分隔最多操作 max 次（默认分割所有匹配成功的位置）

+ `sub(pattern, repl, string, count=0)`

    使用 `repl` 替换所有正则表达式的模式在字符串中出现的位置，除非定义 `count`，否则就将替换所有出现的位置（另见 `subn()` 函数，该函数返回替换操作的数目）

+ `purge()`

    清除隐式编译的正则表达式模式

> **flags**
> 可以一次性指定多个 `flags`，每个之间使用 `|` 进行合并。
>
> + `re.I`, `re.IGNORECASE` 忽略大小写
> + `re.L`, `re.LOCALE` 根据本地语言环境来处理 `\w` `\b` `\s` 等匹配
> + `re.M`, `re.MULTILINE` `^` `\$` 定位的将是每行的开始与结束，而不是整个字符串的开始和结束
> + `re.S`, `re.DOTALL` `.` 原本匹配的是除了 `\n` 之外的所有单个字符，现在可以匹配包括 `\n` 在内的所有字符
> + `re.X`, `re.VERBOSE` 通过反斜线转义，否则所有空格加上 `#`（以及在该行中所有后续文字）都被忽略，除非在一个字符类中或者允许注释并且提高可读性

> **Python 正则对象**
>
> 在上方的函数介绍中，可以发现其中的正则匹配就算没有将字符串转换成正则对象也能实现对应的功能，<u>那么还有没有必要将正则字符串编译成正则对象呢？</u>
>
> 在书中是这样的介绍的：“使用预编译的代码对象比直接使用字符串要快，因为解释器在执行字
符串形式的代码前都必须把字符串编译成代码对象。” 通过这样的结论，应该尽可能将需要使用到的正则表达式转换为正则对象，以此来提高执行的效率。
>
> 之所以能够直接通过字符串来实现正则对象的功能，是因为其中进行了隐式的转换，之后需要使用 `purge()` 函数来清除这部分所占用的资源。

##### 补充知识

###### 字符串标识

在 Python 中，用于描述正则表达式的字符串之前，可以使用 `r` 来进行标识，表示这个字符串中输入的内容是正则表达式。通常的编译器中会内容的内容提供额外的提示信息。

```python
>>> re.split(r', | (?=test)', 'New String test, test My Test')
# 对字符串中的 `, ` 或 ` test` 进行切分
['New String', 'test', 'test My Test']
```

###### 快速使用 flags

Python 中，在定义正则表达式字符串时，额外添加其他的描述，来决定其中所使用到的 `flags`。

在字符串的开始处，使用 `(?[flags])` 的方式，就能直接在定义正则表达式的同时，为其绑定上特殊的 `flags`。

```python
>>> re.split(r'(?i), | (?=test)', 'New String test, test My Test')
# 对字符串中的 `, ` 或 ` test` 进行切分
# 同时，其中指定使用了 `re.I` 忽略大小写
['New String', 'test', 'test My', 'Test']
```

### 网络编程

> Python 中的网络编程，主要围绕的是 socket 来进行

#### 套接字

> 转自
>
> https://cloud.tencent.com/developer/news/478432

套接字是一套用**C语言**写成的应用程序**开发库**，它首先是一个库。主要作用就是实现进程间通信和网络编程，因此在网络应用开发中被广泛使用。

**套接字（socket）**是一个抽象层，应用程序可以通过它**发送或接收数据**，可对其进行像对文件一样的**打开、读写和关闭**等操作。套接字允许应用程序与网络中的其他应用程序进行通信。网络套接字是<u>**IP地址与端口**</u>的组合。

在Python中可以基于套接字来使用传输层提供的传输服务，并以此进行开发网络应用。实际开发中使用的套接字可以分为三类：<u>流套接字（TCP套接字）、数据报套接字和原始套接字</u>。

***

在实际的使用中，Python 中分别指定了两种不同的 `socket_type` 分别用于实现 TCP 传输和 UDP 传输。（之后的 `socket` 中会介绍）

#### socket 函数

> 因为该方法是 Python 网络编程的核心或者说必要的第一步，因此直接放在此处。
>
> 之后会有其他的网络相关函数的介绍。

在 Python 中，网络接口的核心就是 `socket` 模块下的 `socket()` 函数，该函数具体的定义如下：
```python
socket(socket_family, socket_type, protocol=0)
```
其中的参数具体含义如下：
+ `socket_family`

    指的是套接字的家族，目前 Python 中支持 `AF_UNIX` 和 `AF_INET` 两项。
    
    > 在书中的 网络编程>通讯端点>套接字 中有较详细的介绍
    > 
    > 之后可能会出现 `AF_LOCAL` 替换 `AF_UNIX`，`AF_INET6` 替换 `AF_INET` 的情况。
    
+ `socket_type`
    
    用于决定数据连接的类型，分别使用 `SOCKET_STREAM` 代表 TCP 连接，`SOCKET_DRGAM` 代表 UDP 连接。

+ `protocol` （协议，通常省略）

在使用时，可以直接使用 `socket.socket()` 来创建连接，默认情况下使用的是 `AF_INET` 和 `SOCKET_STREAM` 所产生的连接，因此完整的定义应该是 `socket.socket(socket.AF_INET, socket.SOCKET_STREAM)`。

#### 套接字对象函数

根据其中的作用不同，主要需要将其分为服务端和客户端的函数。

##### 服务器套接字函数

+ `socket.bind()`

    将地址（主机名、端口号）绑定到套接字上
    
+ `socket.listen()`

    设置并启动 TCP 监听器

+ `socket.accept()`

    主动接受 TCP 连接，一直等待连接到达（阻塞）

通过以上的函数，可以大致抽象出服务器套接字的整体框架（TCP 连接）：

```python
ss = socket()   # 创建服务器套接字
ss.bind()       # 套接字与地址绑定
ss.listen()     # 监听连接
inf_loop:       # 服务器无限循环
    cs = ss.accept()    # 接受客户端连接
    comm_loop:          # 通信循环
    cs.recv()/cs.send() # 对话（接收/发送）
    cs.close()          # 关闭客户端套接字
ss.close()      # 关闭服务器套接字#（可选）
```

UDP 作为相比起来不安全的连接方式，其搭建的过程也更加简单：

```python
ss = socket()   # 创建服务器套接字
ss.bind()       # 绑定服务器套接字
inf_loop:       # 服务器无限循环
    cs = ss.recvfrom()/ss.sendto()  # 接受/发送数据
ss.close()      # 关闭服务器套接字
```

##### 客户端套接字函数

+ `socket.connect()`

    主动发起 TCP 服务器连接

+ `socket.connect_ex()`

    `connect()` 函数的拓展，以错误码的方式返回，不会报错
    
客户端的模板相比起服务器更加简单，其在建立连接（TCP）之后，只需要持续发送和（或）接受数据即可：

```python
cs = socket()   # 创建客户端套接字
cs.connect()    # 尝试连接服务器
comm_loop:      # 通信循环
    cs.send()/cs.recv()     # 对话（发送/接收）
cs.close()      # 关闭客户端套接字
```

UDP 连接的客户端：

```python
cs = socket()   # 创建客户端套接字
comm_loop:      # 通信循环
    cs.sendto()/cs.recvfrom()   # 对话（发送/接收）
cs.close()      # 关闭客户端套接字
```

##### 通用

+ `socket.recv()`

    接收 TCP 信息
    
+ `socket.recv_into()`

    接收 TCP 信息到缓冲区

+ `socket.send()`

    发送 TCP 信息。因为只会发送一次数据，数据有没有完全发送需要自己接受进行处理（重复调用多次 `send()` 函数），返回的是本次发送的数据包的长度，因此使用判断其长度即可。

+ `socket.sendall()`

    完整得发送 TCP 信息。将原本 `send()` 中需要用户手动进行控制多次执行的过程，其内部提供了实现，最终执行成功返回 `None` 失败报错。

+ `socket.recvfrom()`

    接受 UDP 信息
    
+ `socket.recvfrom_into()`

    接受 UDP 信息存放到缓冲区中

+ `socket.sendto()`

    发送 UDP 信息

+ `socket.getpeername()`

    获取连接到的远程地址

+ `socket.getsockname()`

    当前套接字的地址

+ `socket.getsockopt()`

    返回指定套接字选项的值

+ `socket.setsockopt()`

    设置指定套接字选项的值

+ `socket.shutdown()`

    关闭连接（推荐）
    
    这样的方式将会在断开时主动发送请求，之后正常断开。其他的断开方式通常采用的都是心跳包的方式进行判断。

+ `socket.close()`

    关闭套接字

***

+ `socket.setblocking()`

    设置套接字是否阻塞

+ `socket.settimeout()`
    
    设置超时时间

+ `socket.gettimeout()`

    得到超时时间
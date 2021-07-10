# cocos2d-x

在学习使用 cocos2d-x 过程中对相关的知识点、demo 项目的笔记记录。

## 知识点

> 来自 cocos2d-x 的官方文档
>
> https://docs.cocos.com/cocos2d-x/manual/zh/

### 介绍

cocos2d-x 是 MIT 许可证下发布的一款功能强大的开源游戏引擎。其最大的优势在其<u>**开发简单**，同时有很强的**跨平台性**。</u>

在其中，支持使用 `C++`、`Javascript` 及 `Lua` 三种语言进行游戏的开发。

在其中主要具有以下的特点：

+ 现代化的 `C++` API
+ 立足于 `C++` 同时支持 `Javascript/Lua` 作为开发语言
+ 可以跨平台部署，支持 iOS、Android、Windows、maxOS 和 Linux
+ 可以在 PC 端完成游戏的测试，最终发布到移动端
+ 完善的游戏功能支持，包含<u>精灵、动作、动画、粒子特效、场景转换、事件、文件 IO、数据持久化、骨骼动画、3D</u>

### 基础功能

如上方的介绍，在 cocos2d-x 中，存在精灵、动作、场景、UI组件的基础功能。

#### 精灵

精灵（Sprite）就是屏幕上移动的对象，其中就包括游戏的主角，即用户操作的对象。**但是**，其中需要注意，<u>并不是所有的图形对象都是精灵，精灵特指的是可以对其进行操作（进行控制，或是发生变化）的对象，而无法控制的对象，那只是一个节点（Node）。</u>

准确来说，<u>*精灵是一个能够通过改变自身属性：角度、位置、缩放、颜色等，变成可控制对象的2D图像。*</u>

##### 创建精灵

在 cocos2d-x 中（或者说2D游戏中），使用的是图片来代变某个具体的对象，此处支持使用 `PNG`、`JPEG`、`TIFF`、`WebP` 格式的图片。

> 此外，还支持使用 **图集** 、**精灵缓存** 来创建

###### 图像创建

`Sprite` 能够使用具体的某个图像进行创建：

```cpp
auto mySprite = Sprite::create("mysprite.png");
```

通过指定某个对象所对应的图像，来创建该精灵对象。这样的方式所创建的对象，会直接整个对象来生成，对象的分辨率会直接决定该精灵对应的分辨率。

如果需要的是截取图像中的部分资源来生成对象，就可以在其中指定一个矩形的方式，对图像进行截取，最终的精灵也会按照截取的大小进行生成。

> **注意**
>
> 此处的截取是确定两个点，分别是左上的坐标以及右下的坐标。

###### 图集创建

在游戏行业中，常见到将多张单独的图片存放到一起，形成一整张图片，这样的方式存放更加节省空间，同时提高了读取的速度。

在 cocos2d-x 中，可以将图集加载到全局的缓存（精灵缓存） `SpriteFrameCache` 中，之后可以通过名称来读取对应的图集，再使用其中的

## Demo 练习

### 预先准备

> 此处的主要内容参考:
>
> [【孙老师】Cocos2d-X初见课程-30分钟教你写个小Demo](https://www.bilibili.com/video/BV1WW411A7xZ?from=search&seid=4407063940565964291)
>
> [【博毅创为】【Creator开发教程】【游戏开发】由浅入深，菜鸟也能做游戏【Cocos Creator】](https://www.bilibili.com/video/BV1pb411G7aR?from=search&seid=9931499797631893172)

#### 搭建环境 (Visual Studio 2017)

> 此处使用的环境是 `Visual Studio 2017` 在其中的 `cmake` 对应的是 `Visual Studio 15 2017`

之后的开发都将使用 `C++`，在 Visual Studio 2017 中进行开发，根据官方的文档首先需要生成对应项目的 VS 解决方案（`slt` 文件）。

1. 开启 cmd 控制面板
2. 移动到 cocos 下的 `tools\cocos2d-console\bin` 目录
3. 使用命令 `python cocos.py new [项目名] -l [开发语言] -d [项目目录]` 将会在对应目录生成项目的基本模板文件
4. 之后进入所生成的项目目录（项目根目录），为了方便使用，再创建一个目录 `mkdir win32_build`，进入该目录，再使用 `cmake .. -G "Visual Studio 15 2017"` 来生成 Visual Studio 中的解决方案 `.sln` 文件，双击打开就能在直接开启
5. 右键解决方案，点击**属性**，调整其中的启动项目为此处所编辑的项目。（这是使用 `cmake` 生成的项目中会出现的问题）

> **注意事项**
>
> cocos 此处所提供的 `.py` 文件（`tools\cocos2d-console\bin` 下的 `cocos` 文件），其中使用的是 `python2` 的代码，因此如果 `python3` 会出现报错。

#### 搭建环境 (Cocos Dashboard)

> 可以使用官方提供的开发工具 `Cocos Creater` 在其中安装了插件的情况下相当于内置了 `Visual Studio Code` 因此也是十分好用的开发工具

1. 在 cocos 官网上下载 `Cocos Dashboard`，其中可以集成下载 `Cocos Creater` 使用的效果类似 `Unreal Engine` 与 `Epic` 游戏平台
2. 在其中使用创建项目

## Cocos Creater

简单介绍 Cocos Creater 中的各种功能，以及需要注意的内容

### 常见设置

在 Cocos Creater 中，存在两个不同的配置，分别是全局的设置（针对 Cocos Creater 的设置）和针对项目的设置

#### 全局设置

> 在 `文件 > 设置` 所开启的设置

+ 设置外部编辑器（数据编辑）

    其中主要需要注意的是可以配置外部的编辑器，一般指定使用的是 `Visual Studio Code`
    
+ 设置原生环境（原生环境开发）

    在其中，<u>设置了什么环境就能开发出什么环境下的游戏</u>。默认情况下内置了 Javascript 环境，此外在 `Windows` 环境下还可以开发安卓平台的游戏，但是需要在此处的配置 `Android SDK` 环境。
    
#### 项目设置

> 在 `项目 > 项目设置` 所开启的设置

+ 设置分组（分组管理）

    项目中常需要使用到的分组管理功能（在**碰撞**的相关开发时会使用到）
    
+ 项目模块（模块设置）

    管理项目中需要引入的模块，如果不需要使用到的模块可以取消勾选，从而不会引入对应的模块到项目中
    
+ 项目预览（项目预览）

    其中可以设置项目的预览相关功能，主要是可以设置预览时打开的场景（比如项目中存在登录功能，需要每次正常的运行必须先登录，就可以设置默认项目预览登录场景）
    
#### 构建发布

> 在 `项目 > 构建发布` 中配置

项目最终的打包生成

+ 初始场景

    打包后项目的运行的初始目录
    
+ 发布路径

    默认项目将会在当前目录生成 `build` 文件夹，并在其中生成最终的文件。
    
### Demo 飞机项目

> 实现一个跟随鼠标点击移动的飞机，飞机能够每隔 0.1 秒发射子弹。

> 项目名称 `demo_game_project`

通过使用 Visual Studio 打开项目，找到其中的 `demo_game_project`，打开其中的 `Source File > Classes`，打开其中的 `HelloWorldScene.cpp` 和 `HelloWorldScene.h`。

1. 在 `HelloWorldScene.h` 中添加一个成员属性 `cocos2d::Sprite* plane` 表示用户控制的飞机精灵。
2. 在 `HelloWorldScene.cpp` 中来实例化对象，并将飞机的精灵添加到场景中。
    ```cpp
    plane = Sprite::create("GodPlane.png"); // 指定某个飞机的图片，来创建对象
    if (plane) {    // 如果对象为非空，设置其初始位置，并添加到场景中
        // 指定对象的初始位置在界面中央
        plane->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));   
        // 将精灵添加到场景中
        this->addChild(plane, 1);   // 此处指定为1层（数值越大越上层，覆盖下层），防止之后子弹覆盖飞机图层
    } else {    // 创建对象失败之后的报错信息
        problemLoading("'GodPlane.png'");
    }
    ```
3. 为精灵添加监听器（`Listener`）使其跟随鼠标移动。
    ```cpp
    // 创建鼠标点击对象
    auto touchListener = EventListenerTouchOneByOne::create();
    // 为其添加鼠标点下时的事件监听
    touchListener->onTouchBegan = [&](Touch* touch, Event* event) {
        // 获取当前鼠标点击的位置
        // 但是，直接获取到的坐标使用的是笛卡尔坐标系，需要转换为对应的GL坐标系
        Point p = Director::getInstance()->convertToGL(touch->getLocationInView());
        // 让飞机精灵移动到对应的位置
        plane->runAction(MoveTo::create(0, p));
        return true;
    };
    // 为其添加鼠标拖动的事件监听
    touchListener->onTouchMove = [&](Touch* touch, Event* event) {
        // 其使用的逻辑一致，将飞机精灵移动到当前位置
        Point p = Director::getInstance()->convertToGL(touch->getLocationInView());
        plane->runAction(MoveTo::create(0, p));
    };
    // 将监听器注册到事件调度员上(_eventDispatcher)
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, plane); // 指定具体的对象
    ```
4. 实现定时发射子弹的功能，需要使用到 `cocos` 中的定时器，同时还需要传入一个函数，此处再在头文件中定义一个成员函数。
    ```cpp
    /* HelloWorld.h */
    // 添加了一个成员函数
    void Update(float);   // 接受一个表示回调时间的 float
    ```
    在实现类中完成上方的函数
    ```cpp
    void HelloWorld::Update(float delta) {
        Sprite* bullet = Sprite::create("bullet.png");
        // 将子弹的初始位置就设置到飞机所在的位置
        bullet->setPosition(plane->getPosition());
        // 为子弹添加一个初速度，使其向上飞去
        // 此处使用的是 MoveBy，表示相对于当前位置，需要移动的方向
        // 指定方向时使用的是 Point(0, 900) 此处的 y 值大于屏幕的大小，因此会飞出屏幕后才消失
        bullet->runAction(MoveBy::create(0.5f, Point(0, 900));
        // 将子弹添加到场景中
        this->addChild(bullet, 0); // 设置到0层，在飞机下层，防止被其覆盖
    }
    ```
    最后，将这个方法使用定时器进行回调。
    ```cpp
    // 通过定时器来调用该函数，表示的是每 0.1 秒调用一次
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorld::Update), 0.1f);
    ```
***
至此，最简单的 **Demo 飞机** 游戏完成

#### 注意事项

其中这个过程中所遇到的问题

1. 在网上的教程中，绑定定时器使用的代码都是
    ```cpp
    this->schedule(schedule_selector(HelloWorld::Update), 0.1f);
    ```
    在 Cocos2d-x 4.0 中出现了报错，表示没有定义的 `schedule_selector`，同时在官方文档中也没有找到直接的描述
    
    在搜索代码时，发现了如今应该使用新的写法（见上方的项目实现过程第4步）
    
2. 修改游戏窗口大小

    在 `AppDelegate.cpp` 中，对其中的 `OpenGLView()` 进行设置
    
    ```cpp
    /*AppDelegate::applicationDidFinishLaunching()*/
    // 在 direct->getOpenGLView(); 所得到的对象进行设置
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("demo_start_project", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
		//glview = GLViewImpl::createWithFullScreen("demo_start_project");
    #else
        glview = GLViewImpl::create("demo_start_project");
    #endif
		glview->setFrameSize(1920, 1080);   // 设置了窗口的大小 1920 * 1080
        director->setOpenGLView(glview);    // 将初始化完毕的窗口设置存入 `Director`
    }
    ...
    ```
    实现全屏，只需要将其中的 `GLViewImpl::createWithRect(..)` 修改成 `GLViewImpl::createWithFullScreen(..)`

### Cocos官网新手项目

> 其中所使用到的资源来自 cocos 官网的 `quick-start` 项目，其中的 `start_project` 理论上完成后的项目应该为 `complete_project`，但是完成的项目在官网上所使用的代码是 Javascript，因此此处使用的是 CPP 对其进行重构
>
> [快速跳转](https://docs.cocos.com/creator/manual/zh/getting-started/quick-start.html)

#### 实现步骤

1. 初始化项目
    
    具体实现见 [预先准备](#预先准备)

2. 修改窗口的大小

    在 `AppDelegate.cpp` 中，对其中的 `OpenGLView()` 进行设置，设置其大小为 `960 * 640`
    
3. 添加背景和地面

    此处使用的是精灵来实现，分别创建了两个精灵对象，设置大小、位置之后添加到场景中。
    
    1. 创建对象
    2. 设置大小
        
        此处设置的大小是直接通过获取背景的大小来进行设置的
        
        ```cpp
        // 以背景的设置为例
        // 通过获取到的 visibleSize 中的 width 和 height 来设置其大小，最终的背景图片应该会根据背景的大小来进行动态的生成
        background->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height));
        ```
        
    3. 设置位置
    
        默认情况下，图片所对应的的锚点（操作点）在其中心，因此只需要将其设置到画面的正中心即可。
        
        此处主要针对设置锚点进行额外的讲解，设置的锚点与之前在生成精灵时指定矩形的逻辑并不相同。生成精灵时指定的矩形需要定义一个点，通过这个点，确定了一个矩形的左上角，之后来决定矩形的大小。而此处指定的锚点，相对的位置是从图片的左下角，因此，如果设置的锚点是 `(0, 0)` 表示将锚点设置到的图片的最左下角，之后的指定位置的时候就需要根据这个位置进行设置。
        
        ```cpp
        // 以地面为例
        // 设置了图片的锚点为 (0, 0) 将其定在了图片的左下角
        ground->setAnchorPoint(Point(0, 0));
        // 指定了图片左下角的位置
		ground->setPosition(Point(0, -20));
        ```
    
4. 添加操作的角色

    添加的角色主要需要注意以下几点：
    
    + 图层应该在地面之下
    + 设置锚点为图片的下边中心（方便操作）
    
    > **特别注意**
    >
    > 此处又涉及到了设置锚点，通过测试，设置锚点确实是从左下角开始的相对位置，但是，此处传入的，其实是其相对于图片的<u>相对比例</u>。
    >
    > 如，此处需要设置的是到图片的下侧中心点，因此，设置的时候应该传入的是 (0.5f, 0) 这样的点。
    
    ```cpp
    hero->setAnchorPoint(Point(0.5f, 0)); // 设置了锚点在图片的下边中心
	hero->setPosition(Vec2(visibleSize.width / 2 + origin.x, 50));  // 设置其初始位置在中心线与地面的交界处
	this->addChild(hero, 0);  // 设置其在地面图层下方
    ```
    
    
#### 特别注意

1. Cocos 中目标点的位置

    在 Cocos 中，所指定的点的相对位置可能会各不相同，如 `cocos2d::Rect` 在生成时，需要指定一个点，这个点确定的是矩形的左上角；`Sprite.setAnchorPoint()` 设置图片锚点函数所设置的位置是图片最左下角的相对位置。
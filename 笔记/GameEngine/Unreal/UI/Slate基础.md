
#UI #基础 #功能学习 

> [!quote] 参考资料
> [1 - Slate 开发 - Modern Graphics Engine Guide](https://italink.github.io/ModernGraphicsEngineGuide/04-UnrealEngine/1.Slate%E5%BC%80%E5%8F%91/)

# 1.基础

Slate 是 UE 中提供的 UI 框架。

## 1.1.核心结构

- `FSlateApplication`：全局单例，所有UI的调度中心；
- `SWindow`：顶层窗口，持有跨平台窗口的实例（`FGenericWindow`），提供窗口相关的配置和操作；
- `SWidget`：小部件，跨分窗口区域，提供自身区域内的交互和绘制事件；

## 1.2.基础使用

在 UE 中，一个基础的 Slate 使用示例如下：

```cpp
auto Window = SNew(SWindow)                         //创建窗口
    .ClientSize(FVector2D(600, 600))                //设置窗口大小
    [                                               //填充窗口内容
        SNew(SHorizontalBox)                        //创建水平盒子
        + SHorizontalBox::Slot()                    //添加子控件插槽
        [                                           
            SNew(STextBlock)                        //创建文本框
            .Text(FText::FromString("Hello"))       //设置文本框内容
        ]
        + SHorizontalBox::Slot()                    //添加子控件插槽
        [
            SNew(STextBlock)                        //创建文本框
            .Text_Lambda([](){                      //设置文本框内容   
                return FText::FromString("Slate");
            })      
        ]
    ];
FSlateApplication::Get().AddWindow(Window, true);   //注册该窗口，并立即显示
```


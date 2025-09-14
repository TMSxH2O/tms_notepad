
 #学习笔记 #UE基础 #基础语法

> [!quote] 学习资料
> [UE5 C++全面上手【1.0】课程概览_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1VE421L7Je/?spm_id_from=333.1387.collection.video_card.click&vd_source=129ed45fd60810580b07c07795218205)

# 1.反射机制

反射，能够记录类型的信息，比如类名、类的大小、类的数据成员、类的成员函数等。在代码中获取类型信息，来实现对类的通用处理逻辑（比如通用的序列化、自动暴露脚本接口等）。

在 UE 中，反射系统作为基础功能，在它的基础上，实现了很多通用的逻辑功能。

- 序列化（Serialize）
- 编辑器（应该是指蓝图）
- 垃圾回收（GC）
- 网络同步（Replicated）
- ... 

## 1.1.实现原理

核心围绕 UBT 和 UHT

```plantuml
participant UBT
participant UHT

UBT -> UBT++: 1.扫描解决方案，找出包含的模块和插件
UBT -> UBT: 2.决定了哪些模块需要被重新构建
UBT -> UHT++: 3.触发 UHT 解析C++头文件
UHT -> UHT: 3.1.解析C++头文件中的反射宏标记
UHT -> UHT: 3.2.生成反射注册的相关代码
UHT -> UHT--: 3.3.将生成的代码存放到特定的目录下，\n交给 UBT 去编译，最后链接到一起
UBT -> UBT: 4.根据 .Build.cs 和 .Target.cs 文件，\n设置编译器和链接器的配置
UBT -> UBT--: 5.执行/调用各个平台的编译器（MSVC、LLVM、GCC等）
```

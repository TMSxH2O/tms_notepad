> PlantUML 中的一些比较使用的代码

# 通用

在任意的图中都能生效

## 调整整体字体

> https://plantuml.com/zh/skinparam

```
'设置Class中字体为宋体，整体生效，此处能设置的值根据机器不同而不同
skinparam ClassFontName 宋体
'设置Class中字体大小为12
skinparam ClassFontSize 12
'设置Class中字体颜色为红色
skinparam ClassFontColor red
```

其中第二个参数用于指定修改的是某个中的值，如果想通用一点可以设置 `Defaultxxx`，如 `DefaultFontName`

> 具体的参数列表在链接中有罗列
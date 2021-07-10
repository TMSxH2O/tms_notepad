> 阅读《现代CPP教程》（《modern-cpp-tutorial》）的过程中，所产生的笔记

# 类型推导

在C++中，存在`auto`和`decltype`两个关键字，用于实现类型推断。

- <span style="color:red">**auto**</span><br>对变量进行类型推断，通过变量的值反向推理变量的类型，将会得到一个更加合理（或者说常用）的类型。<br>如会自动忽略顶层`const`、忽略引用等。
- <span style="color:red">**decltype**</span><br>为了解决`auto`关键字只能对变量进行类型推导的缺陷而出现的。其使用的方法与`typeof`类似。可以对简单的函数结果进行类型推断，同时可以保证得到的是变量原本的类型。
    > 可以通过添加额外的一层小括号，来保证最终的结果是引用类型

## 尾返回类型推导

为了解决，之前的C++中，如果想编写比较灵活的代码，如并不确定函数的返回值类型的情况，将会导致代码变得十分丑陋。如利用向上转型等方式，之后再判断类型并尝试向下转型（`dynamic_cast`）。

此时，可以使用类型推导来实现更加自然得对函数返回值类型进行推导。

```
// C++11中的写法
template<typename T, typename U>
auto function(T x, U y) -> decltype(x+y) {
    return x+y;
}

// C++14中直接支持了普通函数具备返回值类型推导
template<typename T, typename U>
auto function(T x, U y) {
    return x+y;
}
```

## 参数转发

在C\++中，对函数的嵌套封装过程中，必须要关注原函数的参数以及返回值，增加了代码的工作量（判断函数的返回值）。在**C\++14**中，主要用于对转发函数或封装的返回值类型进行推导，让我们可以无需显示指定`decltype`的参数表达式。

```
// 需要对以下两个函数进行封装
std::string  lookup1();
std::string& lookup2();

// 在C++11即以前的版本中，将使用这样的方式进行封装
std::string look_up_a_string_1() {
    return lookup1();
}
std::string& look_up_a_string_2() {
    return lookup2();
}

// 而在C++14中，可以直接使用decltype(auto)完成参数转发
decltype(auto) look_up_a_string_1() {
    return lookup1();
}
decltype(auto) look_up_a_string_2() {
    return lookup2();
}
```

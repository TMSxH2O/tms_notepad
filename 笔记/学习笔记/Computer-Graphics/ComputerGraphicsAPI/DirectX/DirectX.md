## 拓展

### 学习思路

> https://segmentfault.com/q/1010000004456104
>
> by lijipeng787 更新于 2016-02-19


目前来讲，龙书已经出版了以下四本（四本均有网络资源可下载）：
1. 《Introduction to 3D Game Programming with DirectX 9.0》（有中文翻译，可下载，不推荐，过于远古）
2. 《Introduction to 3D Game Programming with DirectX 9.0c: A Shader Approach》（介绍可编程流水线）
3. 《Introduction to 3D Game Programming with DirectX 10.0》（有中文翻译，可下载）
4. 《Introduction to 3D Game Programming with DirectX 11.0》（无中文翻译，可下载）
5. 《Introduction to 3D Game Programming with DirectX 12.0》（预计今年5月份出版）

其中第三本与第四本的区别在于计算着色器和曲面细分着色器的介绍（DirectX 11 引入的两个最新特性），其他方面第三本与第四本基本一致，只是章节分布上略有不同，而且dx11相比dx10的API变化并不明显，如果第四本的英文暂时无法有效的阅读的话，可以先阅读第三本的中文版，完成书中的sample，再看第四本，如果英语没问题的话，建议直接看第四本。

如果第四本能够基本看明白，并且已经有了一定的代码积累的话，建议一定要看一下这一本：`《Practical Rendering and Computation with Direct3D 11》`

内容跟msdn有的一拼，甚至更详细，有助于你了解d3d这个api的使用机制，知道为什么龙书里要这样写，为什么这个flag没有用到，什么时候这个flag应该开启，如果dx11龙书能完完整整的看下来，而且代码上也没什么问题，强烈推荐接下来看这本书。

中文书的话，推荐浅墨的csdn博客，比较全面的介绍了dx11的编程，并且已经出版，不过内容与上面几本书基本一致，但是还是推荐龙书。

另外还有两本中文书，一本似乎叫做《DirectX 11 高级编程》，蓝白相间的书皮。就像名字暗示的那样不靠谱，本书基本上就是从msdn和网上摘抄了一些文档，极度不推荐。

另一本叫做《DirectX 3D HLSL高级编程》，这本书是国内罕见的介绍图形编程的书里面罕见的内容有营养的书，不过使用的是微软的effect框架。可惜的是，全书使用了dx9 的effect框架开发，只有在后半部分简单的介绍了dx11的相关知识，题主所说的dx 2010 sdk中包含的大量sample全部是基于effect框架编写，如果想要看懂本书的sample，也需要先学习下微软的effect框架了。

***

最后，推荐这两个网站

http://www.rastertek.com/index.html

http://www.braynzarsoft.net/index.php?p=DX11Lessons

目前全网最全的dx11教程，由浅入深，并且全部基于dx11 api开发，没有使用任何第三方的组件。

***

最近开始学习directX 12 目前已经将上述第一个参考网站的部分demo移植到了dx12上。过一段时间，希望可以有时间将自己的学习心得分享出来:)

ambient lighting：
![ambient lighting](https://segmentfault.com/img/bVsRFm)

specular lighting:
![specular lighting](https://segmentfault.com/img/bVsRFH)
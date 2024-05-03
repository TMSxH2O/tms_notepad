> 主要参考 《Forward+: Bringing Deferred Lighting to the Next Level》

# 摘要

论文的目的是给出一个可以用于渲染大量光源的前向渲染管线，同时不存在延迟渲染管线占用带宽过大的问题。

## 整体流程

Forward+在前向渲染的最终着色阶段前，增加了一个光源剔除阶段。因此，使用Forward+方法的渲染管线大致流程：

1. depth prepass 深度预处理
2. light culling 光源剔除
3. shading 最终着色

深度预处理是前向渲染的一个可选阶段，但是该阶段可以有效减少后续需要处理的面（或者说像素）。
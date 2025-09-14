
> [!quote] 参考资料
> [GDC Vault - Fitting the World: A Biomechanical Approach to Foot IK](https://www.gdcvault.com/play/1023009/Fitting-the-World-A-Biomechanical)

基于育碧 GDC 2016 年的分享，在UE中复现效果。

目前考虑的核心部分应该至少包括三个部分：

1. 【前期准备】提前处理动画序列，生成步幅曲线（每次跨步过程中，脚骨骼到落地点的距离）；
2. 【运行过程】射线检测，进行脚骨骼贴地（传统脚步IK）；
3. 【运行过程】运动过程中，根据当前脚步距离，生成曲线（为了保证顺畅，需要进行平滑处理，生成向上的凸折线）；

> [!todo] 优化方向
> 理论上2、3是可以进行合并的

# 1.对照脚步IK

用于回答 “为什么需要预测脚步IK” 它的优势在哪里。

之前的IK脚骨表现如下，本质上是根据当前脚骨所在的坐标，叠加上地表相对高度。因为这种做法采用的是即时性的更新，判断脚骨正下方这一个点的高度，导致在地面高度突然变化时，脚骨需要突然进行不自然的大幅度提升/下降。

就算使用了插值限制脚骨的提升/下降幅度，依然效果不是很理想。

> [!example] 图例
> - 白色的折线表示可以踏上的地面；
> - 红色的曲线表示脚骨骼在跨步过程中经过的弧线；
> - 蓝色的曲线表示 **预测曲线**，表示脚骨骼需要额外提高的高度；
> - 紫色曲线表示最终脚骨预期会走过的曲线；

> [!info]- 完整的流程
> 
> 每帧在脚骨位置（XY使用脚骨坐标，Z使用根骨骼位置）发射射线，往重力方向检测，检测出脚骨位置与根坐标高度的差值，将这个高度差补到脚骨上。
> 虽然这里的修改通常会使用弹簧差值（或线性差值）让脚骨移动变得更加连贯，但本质上并没有解决脚骨会根据场景位置进行跳变的问题（而且差值也会导致脚穿到地面内）。

![[PredicateFootIk-基础脚步IK表现.gif|500]]

使用预测脚步后，会额外引入一条变化更加流畅的预测轨迹，提前调整脚骨位置。

![[PredicateFootIk-预测脚步IK效果演示.gif|500]]

# 2.预测脚步

这里选择了在 Motion Matching 的 Trajectory（轨迹）插件基础上改造实现。同样分为了两个核心的节点：

- `Predicate Foot Generate Trajectory`（参考 `Pose Search Generate Trajectory` 节点实现） 用来初始化生成 `Trajectory`，根据当前的速度（用来确定轨迹方向）、跨步距离（用来确定轨迹的长度），生成预测的采样点。
- `Handle Foot Predication Trajectory World Collisions` （参考 `Handle Trajectory World Collisions` 节点实现）在轨迹的采样点上，进行射线检测使用采样点贴合地面，再进行处理让轨迹变得平滑。

## 1.1.预测起始点未继承状态

因为使用方式与 Motion Matching 中一致，是每帧都会进行更新，导致每次脚步贴地都会把当前采样点贴地，导致预测的位置低于预期，不能呈现逐步上升的效果。如下方的蓝色线条。

理想的情况肯定是如红线所示，必须提供上一帧的状态，才能保证贴地后的采样点表现正常。

> [!question] 疑问
> 实际按照 Motion Matching 节点原本的做法，利用上一帧世界坐标来定预测的起始点，理论上可行。
> 但测试表现并不理想，腿骨依然会出现明显的跳跃现象。

![[PredicateFootIk-错误效果-未继承状态.gif|500]]

# 3.脚骨贴地

参考 ALS（AdvancedLocomotionSystemV4）

调整骨骼位置（盆骨位置）发生在 **ALS_AnimBP** 的 FootIK 动画层。

![[UE-Animation-ALS-FootIK.png|400]]

其中，调整盆骨位置，是利用 **变换（修改）骨骼** 节点，让骨骼整体下移，可以更自然地踩在地面上。

这里地 `PelvisOffset` 是在 `SetPelvisIKOffset` 函数中计算得到，找出两个脚骨往下调整更多的一边，作为盆骨的Offset

![[UE-Animation-ALS-SetPelvisIKOffset.png|500]]

在这个基础上，为了实现预测脚步，沿着预期的轨迹抬起或降低脚骨位置，在计算 `FootOffset` 时，会额外加上 `Trajectory` 预测的 `HeightOffset`。

--- 

另一个更新、更现代的做法，是通过 `Control Rig` 实现，本质上没什么区别。

主要的麻烦在于 `Control Rig` 内只支持骨骼空间（或者说组件空间）或父骨骼空间，而预测的脚骨位置通常是世界坐标，处理起来相对会比较麻烦。而且 `Control Rig` 内部的计算，想调试相对会更加麻烦，暂时只能靠 `Draw Debug` 绘制一些调试的图案来辅助查bug，还是不太直观。

> [!todo] 待优化
> 测试过程中，使用 `Control Rig` 实现的版本一直不好定位bug，经常出现奇怪的抖动，所以换回了Game线程的 Update 里计算。

# 待处理问题

 - [ ] 如何解决处理每帧调用 `HandleFootPredicationTrajectoryWorldCollisions` 会导致预测的脚骨目标位置贴合地面

思路1：<s>使用History的位置？History里最后一个采样点正好就是当前位置。</s>似乎不行，IK骨骼的位置就像是没有被经过贴地处理，总是比实际的位置高出1.0cm。待验证：每帧动画更新前，获取到的IK骨骼位置是不正确的吗？

可以每帧更新Trajectory前，先根据当前的曲线值（上一帧的值），来获取到上一帧的OffsetHeight，设置给第一个点，就能保证预测的位置不会每次都从0开始。
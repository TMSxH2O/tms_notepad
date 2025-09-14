
主要用于MotionMatching中，生成预测的曲线数据，在UE中，是一个单独的插件。

# 使用

> 这里只考虑它的初始化、数据更新，不考虑MotionMatching中的完整使用。

核心节点主要有：
- `Pose Search Generate Trajectory`<br>【可以视作初始化接口】。用于生成 Trajectory 数据（`FPoseSearchQueryTrajectory`），根据配置创建对应数量的 History 位置数据和 Predicate 位置数据。
- `Handle Trajectory World Collision`<br>用于在 Predicate 位置上进行物理碰撞检测，处理碰撞后的关键点位置（生成翻阅障碍的曲线）。

## Pose Search Generate Trajectory

内部逻辑梳理

```plantuml
start
:FPoseSearchTrajectoryData::UpdateData;
:UPoseSearchTrajectoryLibrary::InitTrajectorySamples;
:UPoseSearchTrajectoryLibrary::UpdateHistory_TransformHistory;
:UPoseSearchTrajectoryLibrary::UpdatePrediction_SimulateCharacterMovement;
end
```

`FPoseSearchQueryTrajectory` 的 `Samples` 成员变量存储的是采样点的数据，其中前 `NumHistorySamples` 个对象是 History 采样点，之后的就是 Predicate 的采样点。

通常都使用的是 `Pose Search Generate Trajectory (for Character)` 节点，但这个节点要求必须传入 Character 或 AnimInstance 来间接获取到 Character 对象，具体是会使用其中的 `CharacterMovementComponent` 和 `SkeletalMeshComponent` 来填充 `FPoseSearchTrajectoryData::FDerived` 的数据。

---

直到UE5.5，还提供了一个实验节点 `Pose Search Generate Trajectory (using Predictor)`，通过用户自定义的 Predictor （继承 `IPoseSearchTrajectoryPredictorInterface` 接口）来生成 Trajectory 数据。可以观察官方实现 `UMoverTrajectoryPredictor`，来分析接口所需的逻辑。

- `Predict` 传入了预测采样点的数量 `NumPredictionSamples`、每个采样点之间的间隔 `NumPredictionSamples`、历史采样点的数量 `NumHistorySamples`。用来更新 `FPoseSearchQueryTrajectory` 里的采样点数据（位置 `Position`、旋转量 `Facing` 和预测点时间 `AccumulatedSeconds`）

> [!todo] 思路整理
> <s>如果想在 Trajectory 基础上，想实现脚步的预测，只需要在 `AnimationModifier` 处理的时候生成一条关于脚骨骼位置的 Transform 曲线就可以？</s>
> 
> 验证后发现不可行，有两方面原因：
> 1. 动画蓝图运行过程中，可能不止一条曲线同时作用；
> 2. AnimInstance 虽然也能获取当前曲线值，一方面是只能获取当前曲线值，另一方面是只能获取 float 曲线值，Transform 曲线没有支持（具体的实现参考 `UAnimSequence::EvaluateCurveData` 使用的 `EvaluateFloatCurvesFromModel` 只导出 float 曲线；父类的实现也是，调用 `FRawCurveTracks::EvaluateCurveData` 只导出 float 数据，通过注释信息 Transform 曲线应该只用于编辑器环境）；
> 
> 考虑后只是用 float 曲线以及只基于当前的曲线值对未来的脚步位移进行预测：
> 
> - 遍历脚骨高度，找出所有的落脚点；
> - 根据落脚点，生成一条脚骨距离逻辑点的距离；
> 
> 通过距离

### UpdateHistory_TransformHistory

更新历史采样点数据，流程可以分为：

1. 将历史采样点转换为相对上一帧预测点的局部坐标；
2. 更新历史采样点数据，给所有点加上当前速度计算的位移值，同时淘汰掉旧的点；
3. 将局部坐标转换到相对当前坐标的世界坐标点；



## Handle Trajectory World Collision



> [!quote] 项目链接
> [SPARK-inc/SPCRJointDynamicsUE4: 布風骨物理エンジン](https://github.com/SPARK-inc/SPCRJointDynamicsUE4)

SPCR Joint Dynamics UE4 是一个专门用于UE设计的插件，旨在提供真实高效的动力学解决方案。

> [!info] github仓库描述
> 
> Are you searching for a real cloth dynamics solution, so that character's cloth in your game will behave like a real cloth. Then this is the plugin for you. SPCRJointDynamics offers you the exact behaviour, which requires a basic bone setup, and you are ready to go.

SPRC 使用了交叉仿真算法（弹簧质点系统）实时为骨骼制作动画。==最终的效果主要是关注整体的表现，而不适用于非常精确（毛发、裙子的细节穿插）的物理模拟表现。==本质是根据指定的骨骼构建网格，因此无法避免实际的Mesh穿插问题。

# 1.流程分析

分析核心的动画节点 `FAnimNode_SPCRJointDynamics` 实现

## 1.1.构建流程

动画节点每帧的 `FAnimNode_SPCRJointDynamics::EvaluateSkeletalControl_AnyThread`，通过判断是否需要重置相关数据，如果需要重置（或者初始化）就执行重建逻辑。

```plantuml
start
' 重建逻辑分析
partition "FAnimNode_SPCRJointDynamics::OnResetAll" {
:CreateControlPoints;
note right
创建控制点
end note
:CreateConstraints;
note right
创建约束
end note
:CreateColliders;
note right
创建碰撞器
end note
:CreateSurfaceConstraints;
note right
创建表面约束<color:red>(?)</color>
end note
:初始化碰撞器的位置;
}
end
```

> [!info] 触发重置（`FAnimNode_SPCRJointDynamics::OnResetAll`）的时机
> 
> - 通过指定的 `USPCRJointDynamicsComponent` 组件对象，通过 `isReset` 成员变量控制。在 `FAnimNode_SPCRJointDynamics::RequireResetCheck` 将 `_bRequireResetPose` 修改为 `true`。
> - 动画的状态切换时（进出布娃娃模式时）触发。`FAnimNode_SPCRJointDynamics::ResetDynamics` 会将 `_bRequireResetPose` 修改为 `true`。
> - 节点初始化时触发。`FAnimNode_SPCRJointDynamics::Initialize_AnyThread` 内会修改 `FAnimNode_SPCRJointDynamics::_bRequireResetPose` 为 `true`。

> [!question] 思考：为什么碰撞体数据保存了两份？
> 
> 问题的背景补充：在 `FAnimNode_SPCRJointDynamics::InitializeColliderPositionOld` 函数实现中可以看到。在节点本身，保存了一份数据 `FAnimNode_SPCRJointDynamics::_Colliders` （成员变量），最后依然复制了一份到类静态变量 `FAnimNode_SPCRJointDynamics::_GlobalColliders` 中。
> 
> **这是否就需要更新两次数据，造成浪费？**
>
>  ==TODO==

### 创建控制点

> 源代码 `FAnimNode_SPCRJointDynamics::CreateControlPoints`

通过 `FAnimNode_SPCRJointDynamics::JointPairs` 执行的多条平行的骨骼链，构建控制点 `SPCRPoint`。

简化逻辑分析：内部根据指定的起始、结束骨骼，构建骨骼链，存储了控制点的父子关系、世界空间坐标，以及部分模拟参数（软硬度、权重、重力权重等）。如果指定了虚拟骨骼长度大于0，会在结束骨骼后，继续补充虚拟控制点。

> [!warning] 注意
> `JointPairs` 中指定的起始骨骼到结束骨骼之间的骨骼链之间，为了保证构建出的弹簧质点系统稳定，应该尽量平行（常见的都是多条顺着裙摆方向）的骨骼。
> 
> 让当前骨骼链和相邻骨骼链的相同索引组合，可以形成接近矩形的结构，模拟的效果会更加稳定。

```cpp
void FAnimNode_SPCRJointDynamics::CreateControlPoints(FComponentSpacePoseContext& Output)
{
	TArray<TArray<FCompactPoseBoneIndex>> BoneIndicesTbl;
	
	...

	// 遍历骨骼链，将骨骼点记录到 BoneIndicesTbl 中
	for (auto&& Pair : JointPairs)
	{
		TArray<FCompactPoseBoneIndex> BoneIndices;

		const FCompactPoseBoneIndex RootIndex = Pair.RootBone.GetCompactPoseIndex(BoneContainer);
		FCompactPoseBoneIndex BoneIndex = Pair.EndBone.GetCompactPoseIndex(BoneContainer);
		
		// 将起始骨骼到结束骨骼的索引记录到数组中
		do
		{
			// 确保不会出现重复的骨骼
			if (CheckSameBone(BoneIndex))
			{
				return;
			}
			BoneIndices.Insert(BoneIndex, 0);
			BoneIndex = Pose.GetParentBoneIndex(BoneIndex);
		} while (BoneIndex != RootIndex);
		BoneIndices.Insert(BoneIndex, 0);

		BoneIndicesTbl.Add(BoneIndices);
	}

	// 根据收集到的 BoneIndicesTbl 骨骼索引，创建 SPCRPoint 数据对象
	for (auto&& Indices : BoneIndicesTbl)
	{
		const int32 IndicesNum = Indices.Num();
		if (IndicesNum < 2) continue;

		TArray<SPCRPoint> Points;

		for (int32 iIndex = 0; iIndex < IndicesNum; ++iIndex)
		{
			const auto& BoneIndex = Indices[iIndex];
			const auto Weight = iIndex <= FixedPointIndex ? 0.0f : 1.0f;

			SPCRPoint Pt;
			...  // 收集 SPCRPoint 数据
		}

		// 如果设置了 VirtualBoneLength 大于0，表示需要追加虚拟点
		// 虚拟点可能是为了让末梢的表现更加自然，增加一些拖拽的感觉
		...

		// 控制点数据记录到 _PointsTbl 中
		_PointsTbl.Add(Points);
	}

	// 控制点初始化
	for (auto&& Points : _PointsTbl)
	{
		const int32 PointNum = Points.Num();
		// 初始化控制点的世界坐标
		for (int32 iPoint = 0; iPoint < PointNum; ++iPoint)
		{
			auto& Point = Points[iPoint];

			if (Point.bVirtual)
			{
				// 虚拟点是根据长度和之前的两个控制点方向，计算出位置
				...
			}
			else
			{
				const auto& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(Point.BoneIndex);
				const auto BoneTransformInWorldSpace = BoneCSTransform * Output.AnimInstanceProxy->GetComponentTransform();

				Point.Position = BoneTransformInWorldSpace.GetLocation();
			}

			Point.PositionPrev = Point.Position;
			Point.InitialWorldPosition = Point.Position;
		}

		// 父控制点持有子控制点的指针
		// 看起来是由于这个逻辑，不得不多遍历一次
		for (int32 iPoint = 0; iPoint < PointNum - 1; ++iPoint)
		{
			Points[iPoint].pChild = &Points[iPoint + 1];
		}

		// 子控制点持有夫控制点的指针，同时保留父子节点之间的相对距离
		for (int32 iPoint = 1; iPoint < PointNum; ++iPoint)
		{
			Points[iPoint].pParent = &Points[iPoint - 1];
			
			Points[iPoint].DistanceFromParent = (Points[iPoint].Position - Points[iPoint].pParent->Position).Length();
		}
	}

	// 根据骨骼的索引进行排序，确保后续按照骨骼的索引顺序进行遍历更新控制点位置
	for (auto&& Points : _PointsTbl)
	{
		for (auto&& Point : Points)
		{
			if (Point.bVirtual) continue;
			_PointAccessTbl.Add(&Point);
		}
	}

	_PointAccessTbl.Sort(
		[](const SPCRPoint& lhs, const SPCRPoint& rhs)
		{
			return lhs.BoneIndex < rhs.BoneIndex;
		}
	);
}

```

### 创建约束

> 源代码 `FAnimNode_SPCRJointDynamics::CreateConstraints`

> [!quote] 约束参考图
> ![[MassSpring-Constraint.png|400x]]
> by *SPCRJointDynamics仓库的介绍界面*

根据不同的开关，构建以下约束：

- **水平弯曲约束（`bBending_Horizontal`）**<br>间隔一个骨骼链的相同索引骨骼间，构建约束。如果开启了 `bJointLoop` 会额外增加收尾骨骼链（还是间隔了一个骨骼链）之间的约束。
- **竖直弯曲约束（`bBending_Vertical`）**<br>同一条骨骼链内，在间隔一个骨骼的两个骨骼间，构造约束。
- - **剪切约束（`bShear`）**<br>当前骨骼链相邻骨骼，与响铃骨骼链的对应索引的两个骨骼，总计四个骨骼，组成一个四边形，在这个四边形的两组对角上，增加约束。
- **水平结构约束（`bStructural_Horizontal`）**<br>相邻的骨骼链的相同索引骨骼间，增加约束。如果开启了 `bJointLoop` 会额外增加收尾骨骼链之间的约束。
- **竖直结构约束（`bStructural_Vertical`）**<br>同一条骨骼链内相邻的骨骼间，增加约束

### 创建碰撞器

> 源代码 `FAnimNode_SPCRJointDynamics::CreateColliders`

根据 `FAnimNode_SPCRJointDynamics::Bodies` 构建 `SPCRColliderInfo` 和 `FSPCRCollider`（索引相互对应）。

在 SPCR 中，只提供了两种碰撞器，胶囊体和球体。根据碰撞体的 `Height` 是否接近0来决定。

> [!info] `SPCRColliderInfo`
> 从实现上分析，分析它的定位更倾向一些固定的初始化信息
> 
> - 碰撞器挂载的骨骼
> - 在骨骼局部空间下的偏移量
> - 碰撞器的形状信息（半径和高度）

> [!info] `FSPCRCollider`
> 从实现上分析，分析它的定位更倾向一些更新必要的数据
> 
> - 是否为胶囊体
> - 碰撞器的形状信息（半径和高度）
> - 摩擦系数 **(?)**
> - 位置信息 每帧更新的世界空间坐标
> - 朝向信息 胶囊体中圆柱的方向向量
> - 表面碰撞类型 `SurfaceColliderType` **(?)**
> - 上一帧的位置 `PositionPrevFrame`
> - 上一帧的朝向 `DirectionPrevFrame`
> - 刚才的位置 `PositionPrev` 更新过程中暂存位置信息
> - 刚才的朝向 `DirectionPrev` 更新过程中暂存位置信息

> [!question] 思考：为什么碰撞器的位置初始化需要专门放到 `InitializeColliderPositionOld` 内更新，而不是直接在创建时获取？
> 
> 首先分析 `InitializeColliderPositionOld` 内具体是更新了什么位置信息，实际是更新了 `FSPCRCollider` 内的形状信息（半径和高度）、上一帧的位置和上一帧的朝向。
> 
> 那么有什么情况下，只调用了 `CreateColliders` 而没有调用 `InitializeColliderPositionOld` 呢？
> 目前只有 `FAnimNode_SPCRJointDynamics::SyncColliderPositions` 内有这种情况，用于编辑器环境的编辑模式下，为了方便修改碰撞器的 `Transform` 数据。
> 
> 此时没有开始运行，不会触发动画的更新，所以不需要去初始化位置信息，后续开始运行时再执行初始化即可。

### 创建表面约束

> 源代码 `FAnimNode_SPCRJointDynamics::CreateSurfaceConstraints`

这里的 *表面*，根据实际的逻辑分析，表示的是控制点之间形成的四边形结构。使用 `SPCRSurfaceConstraint` 存储了四个骨骼的 `SPCRPoint` 指针。

## 1.2.更新流程

> 源代码 `FAnimNode_SPCRJointDynamics::EvaluateSkeletalControl_AnyThread`

```plantuml
start
:检测是否需要进行初始化/重置;
note right
如果需要，就会执行前面的初始化逻辑
end note
while (按照120fps的帧率进行模拟)
note right
根据本帧DeltaTime来判断需要循环多少次
end note
#aqua:更新控制点;
note right
重要的更新节点
end note
while (循环指定的模拟迭代次数)
note right
通过 MaxIterations 配置迭代次数
end note
:更新碰撞器;
:更新计算约束;
endwhile
#pink:约束骨骼之间的最大夹角;
note right
可选，通过 bLimitAngle
end note
endwhile
#pink:强制限制长度;
note right
可选，通过 bForceLengthLimit
end note
:将更新后的质点坐标更新到骨骼;
:更新记录本帧碰撞器的位置和方向信息到 PrevFrame;
#pink:绘制测试;
note right
仅在编辑器下，绘制约束、碰撞器和表面约束
end note
end
```

### 更新控制点

> 源代码 `FAnimNode_SPCRJointDynamics::UpdateControlPoints`

```cpp
void FAnimNode_SPCRJointDynamics::UpdateControlPoints(FComponentSpacePoseContext& Output, const FVector& Wind, float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_SPCR_UPDATE_CONTROL_POINTS);

	const auto& ToWorld = Output.AnimInstanceProxy->GetComponentTransform();
	const int32 PointTblNum = _PointsTbl.Num();
	const float StepTime_x2_Half = DeltaTime * DeltaTime * 0.5f;

	// 遍历更新骨骼链上的模拟质点
	for (auto&& Points : _PointsTbl)
	{
		int32 iPoint = 0;

		// 指定 FixedPointIndex
		// 跳过每条骨骼链起始处，对应数量的骨骼更新，固定它们的位置
		while (iPoint <= FixedPointIndex)
		{
			auto& Point = Points[iPoint];

			const auto& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(Point.BoneIndex);
			auto BoneTransformInWorldSpace = BoneCSTransform * ToWorld;

			// 固定位置，但还是需要记录它们的位置
			Point.Position = BoneTransformInWorldSpace.GetLocation();
			Point.PositionPrev = Point.Position;

			++iPoint;
		}

		// 正式开始更新质点位置
		const int32 PointNum = Points.Num();
		while (iPoint < PointNum)
		{
			auto& Point = Points[iPoint];

			// 记录位移趋势
			FVector movementDisplacement = Point.Position - Point.PositionPrev;
			if (bLimitAngle)
			{
				FVector movDispNorm = movementDisplacement.GetSafeNormal();
				movementDisplacement = movDispNorm * FMath::Min(movementDisplacement.Length(), MaxDisplacementLength);
			}

			// 计算质点的位移量
			// ((重力加速度 + 风力加速度) * 0.5 * DeltaTime ^ 2 + 上一帧的位移差值) * 阻力（或者叫衰减系数） * (1 - 摩擦系数)
			FVector Displacement;
			Displacement  = Gravity * Point.Gravity;
			Displacement += Wind;
			Displacement *= StepTime_x2_Half;
			Displacement += movementDisplacement;
			Displacement *= Point.Resistance;
			Displacement *= 1.0f - FMath::Clamp(Point.Friction, 0.0f, 1.0f);

			... // 因为Drag和Damping的行为非常类似，官方将其进行了合并，只考虑Drag

			// 根据移动量，增加阻力
			if (bUseDrag)
			{
				float MoveSpeed = Displacement.SizeSquared();
				FVector VelNormalize = Displacement.GetSafeNormal();
				//instead of -1 can I make the -1/2??
				FVector DragForce = -1 * ((Point.Drag * MoveSpeed * VelNormalize) / 2);

				Displacement += DragForce;
			}

			// 升力，垂直与运动方向，且在运动方向和重力方向形成的平面内
			if (bUseLift)
			{
				FVector MotionVec = Displacement.GetSafeNormal();
				FVector SideVec = FVector::CrossProduct(MotionVec, Gravity.GetSafeNormal());
				FVector LiftVec = FVector::CrossProduct(MotionVec, SideVec);
				FVector ForceLift = LiftVec * Point.Lift;
				Displacement += ForceLift;
			}

			// 记录上一帧的位置，并更新新的位置
			Point.PositionPrev = Point.Position;
			Point.Position += Displacement;
			Point.Friction = 0.0f;

			// 硬度，尝试将控制点调整回骨骼所在的位置
			if (Point.Hardness > 0.0f)
			{
				if (Point.bVirtual)
				{
					auto& Parent = *Point.pParent;
					const auto& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(Parent.BoneIndex);
					auto BoneTransformInWorldSpace = BoneCSTransform * ToWorld;
					auto Target = BoneTransformInWorldSpace.TransformPosition(Parent.CSBoneDirection);
					Point.Position += (Target - Point.Position) * Point.Hardness;
				}
				else
				{
					const auto& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(Point.BoneIndex);
					auto BoneTransformInWorldSpace = BoneCSTransform * ToWorld;
					Point.Position += (BoneTransformInWorldSpace.GetLocation() - Point.Position) * Point.Hardness;
				}
			}

			++iPoint;
		}
	}
}

```

==TODO== 继续完善更新流程

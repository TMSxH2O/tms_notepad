
> 参考[【UE教程】从零开始的Lyra动画蓝图制作+解析（简化版）](https://www.bilibili.com/video/BV1dT41157pS/?p=5&share_source=copy_web&vd_source=31ab01fa448a5b8b25f838871fc101be)

# 学习记录

记录其中的亮点和踩到的坑

## 1. Sequence Evaluator 节点踩坑

动画蓝图中的 `Sequence Evaluator` 节点，必须自己控制播放的时间，在这里使用时，表现为必须在 ”变为相关时(BecomeRelevant)“ 需要把 ”显式时间(ExplicitTime)“ 设置回0；在 ”更新时(Update)“ 自己控制 ”显式时间“ 的更新，内部使用 `AdvanceTime` 节点步进时间，在需要步幅适配时，则使用：

 - `Distance Match to Target`<br>根据传入的距离，修改显式时间
 - `Advance Time by Distance Matching`<br>根据传入的速度，步进调整显式时间

针对 `Sequence Player` 节点，也同样存在距离匹配的需求，与 `Sequence Evaluator` 直接修改当前播放的时间不同，通常是通过控制动画序列的播放速度来达成距离匹配的效果。

 - `Set Playrate To Match Speed`<br>通过修改 `PlayRate` 来进行匹配当前的移动速度。

### Distance Match to Target

用于 `AnimNode_SequenceEvaluator` 序列求值器

核心逻辑实现在函数 `GetAnimPositionFromDistance` 找到距离匹配的时间点。内部是使用二分法的方式，找到最接近目标距离的位置。因此，这里对曲线有潜在的要求，距离曲线必须单调。

```cpp
static float GetAnimPositionFromDistance(const UAnimSequenceBase* InAnimSequence, const float& InDistance, FName InCurveName)
{	
	FAnimCurveBufferAccess BufferCurveAccess(InAnimSequence, InCurveName);
	if (BufferCurveAccess.IsValid())
	{
		const int32 NumKeys = BufferCurveAccess.GetNumSamples();
		if (NumKeys < 2)
		{
			return 0.f;
		}

		// Some assumptions: 
		// - keys have unique values, so for a given value, it maps to a single position on the timeline of the animation.
		// - key values are sorted in increasing order.

		int32 First = 1;
		int32 Last = NumKeys - 1;
		int32 Count = Last - First;

		while (Count > 0)
		{
			int32 Step = Count / 2;
			int32 Middle = First + Step;

			if (InDistance > BufferCurveAccess.GetValue(Middle))
			{
				First = Middle + 1;
				Count -= Step + 1;
			}
			else
			{
				Count = Step;
			}
		}

		const float KeyAValue = BufferCurveAccess.GetValue(First - 1);
		const float KeyBValue = BufferCurveAccess.GetValue(First);
		const float Diff = KeyBValue - KeyAValue;
		const float Alpha = !FMath::IsNearlyZero(Diff) ? ((InDistance - KeyAValue) / Diff) : 0.f;

		const float KeyATime = BufferCurveAccess.GetTime(First - 1);
		const float KeyBTime = BufferCurveAccess.GetTime(First);
		return FMath::Lerp(KeyATime, KeyBTime, Alpha);
	}

	return 0.f;
}

```

### Advance Time by Distance Matching

用于 `AnimNode_SequenceEvaluator` 序列求值器

核心逻辑实现在函数 `GetTimeAfterDistanceTraveled` 找到序列与距离匹配的时间点。内部是采用 1/30s（30帧）的步长往后判断下一个时间点的距离差值，是否等于传入的差值 `DistanceTraveled`。停止的条件时，当某一步的终止位置距离已经大于传入的 `DistanceTraveled` 时，按照比例，计算出合适的位置。核心代码如下：

```cpp
/**
* Advance from the current time to a new time in the animation that will result in the desired distance traveled by the authored root motion.
*/
static float GetTimeAfterDistanceTraveled(const UAnimSequenceBase* AnimSequence, float CurrentTime, float DistanceTraveled, FName CurveName, const bool bAllowLooping)
{
	float NewTime = CurrentTime;
	if (AnimSequence != nullptr)
	{
		// Avoid infinite loops if the animation doesn't cover any distance.
		// 如果距离曲线的范围为0，就算循环处理也没有意义，直接跳过，返回当前时间
		if (!FMath::IsNearlyZero(UE::Anim::DistanceMatchingUtility::GetDistanceRange(AnimSequence, CurveName)))
		{
			float AccumulatedDistance = 0.f;
			float AccumulatedTime = 0.f;

			const float SequenceLength = AnimSequence->GetPlayLength();
			static const float StepTime = 1.f / 30.f;				

			// Traverse the distance curve, accumulating animated distance until the desired distance is reached.
			while ((AccumulatedDistance < DistanceTraveled) && (bAllowLooping || (NewTime + StepTime < SequenceLength)))
			{
				// 记录当前曲线距离和下一步的曲线记录，计算出距离差值
				const float CurrentDistance = AnimSequence->EvaluateCurveData(CurveName, NewTime);
				const float DistanceAfterStep = AnimSequence->EvaluateCurveData(CurveName, NewTime + StepTime);
				const float AnimationDistanceThisStep = DistanceAfterStep - CurrentDistance;

				if (!FMath::IsNearlyZero(AnimationDistanceThisStep))
				{
					// Keep advancing if the desired distance hasn't been reached.
					// 如果距离还不够，就需要往后推进
					if (AccumulatedDistance + AnimationDistanceThisStep < DistanceTraveled)
					{
						FAnimationRuntime::AdvanceTime(bAllowLooping, StepTime, NewTime, SequenceLength);
						AccumulatedDistance += AnimationDistanceThisStep;
					}
					// Once the desired distance is passed, find the approximate time between samples where the distance will be reached.
					// 如果距离已经给够了，这里是假设了一个StepTime之间的距离是直线变化，所以按比例计算出到达目标距离所需的时间
					else
					{
						const float DistanceAlpha = (DistanceTraveled - AccumulatedDistance) / AnimationDistanceThisStep;
						FAnimationRuntime::AdvanceTime(bAllowLooping, DistanceAlpha * StepTime, NewTime, SequenceLength);
						AccumulatedDistance = DistanceTraveled;
						break;
					}
				}
				else
				{
					NewTime += StepTime;
				}
				
				AccumulatedTime += StepTime;
			}
		}
	}

	return NewTime;
}
```

### Set Playrate to Match Speed

用于 `AnimNode_SequencePlayer` 序列播放器

内部是通过对比当前速度与初始状态下（Play Rate 为 1），根运动的速度进行对比，按照比例修改 Play Rate。由于是直接用整个动画序列的移动距离和时间长度来计算得到的平均速度与目标速度匹配，因此，有一个潜在的规则，必须保证整个动画序列过程中的运动速度是稳定的。

```cpp
FSequencePlayerReference UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(const FSequencePlayerReference& SequencePlayer, float SpeedToMatch, FVector2D PlayRateClamp)
{
	SequencePlayer.CallAnimNodeFunction<FAnimNode_SequencePlayer>(
		TEXT("SetPlayrateToMatchSpeed"),
		[SpeedToMatch, PlayRateClamp](FAnimNode_SequencePlayer& InSequencePlayer)
		{
			if (const UAnimSequence* AnimSequence = Cast<UAnimSequence>(InSequencePlayer.GetSequence()))
			{
				// 获取得到当 play rate 等于 1 时的序列时长
				const float AnimLength = AnimSequence->GetPlayLength();
				if (!FMath::IsNearlyZero(AnimLength))
				{
					// Calculate the speed as: (distance traveled by the animation) / (length of the animation)
					// 获取到整个动画序列的根移动距离
					const FVector RootMotionTranslation = AnimSequence->ExtractRootMotionFromRange(0.0f, AnimLength).GetTranslation();
					const float RootMotionDistance = RootMotionTranslation.Size2D();
					if (!FMath::IsNearlyZero(RootMotionDistance))
					{
						// 记录获取到整个动画序列的平均速度
						const float AnimationSpeed = RootMotionDistance / AnimLength;
						// 计算得到目标速度的比例，将比例修改到 play rate
						float DesiredPlayRate = SpeedToMatch / AnimationSpeed;
						if (PlayRateClamp.X >= 0.0f && PlayRateClamp.X < PlayRateClamp.Y)
						{
							DesiredPlayRate = FMath::Clamp(DesiredPlayRate, PlayRateClamp.X, PlayRateClamp.Y);
						}

						if (!InSequencePlayer.SetPlayRate(DesiredPlayRate))
						{
							UE_LOG(LogAnimDistanceMatchingLibrary, Warning, TEXT("Could not set play rate on sequence player, value is not dynamic. Set it as Always Dynamic."));
						}
					}
				}
			}
		});

	return SequencePlayer;
}

```

## 2.蓝图基础踩坑

每次的输入都会重新计算一次输入的值，所以如果这中间有更新会导致之前的结果发生变化，最好就是先Set保存一份久数据，在后面使用。

## 3.八向移动

> ~~计算当前动画朝向时，使用到了 `WorldVelocity`，当角色停下来时，这个值会直接变成0，会导致 ***停止移动*** 状态播放到一半就切换到其他朝向，导致表现异常。~~
>
> 回想起来，其实算是自己的实现有问题，***开始移动*** 和 ***停止移动*** 状态，都应该只在进入状态（`OnBecameRelevant`）时，根据朝向选择播放的动画序列，而不应该在更新过程（`OnUpdate`）中动态切换播放的动画序列。

通过当前的世界空间速度和世界空间旋转，得到角色旋转量（Yaw），返回的值在 \[-180, 180] 之间。其中，-45到45度之间的部分应该表示为向前，-135~135为向后，之后根据值的正负分别表示向右和向左。

为了避免频繁发生转换，官方做了特殊处理，首先是增加了向前和向后的 “死区”（默认是10度），让前后两个方向覆盖的范围更大，同时，从当前状态是前后时，还会让死区范围扩大一倍，使得它更难变成左右方向。

对 ***开始移动*** 和 ***停止移动*** 两种状态，仅在进入状态时判断朝向并切换动画，而不会更新过程中切换；
对 ***循环移动*** 状态，则是在更新过程中，一直判断当前状态，并切换动画序列；

## 4.回转运动

在朝着某个方向还有速度的时候，朝着反方向进行加速，就会进行回转运动。回转运动的动画序列可以视作2个部分：

1. 当前方向的减速，并停止；
2. 朝着反方向进行加速；

> 回转运动的动画序列通常用当前的速度方向，而不是最后的反方向作为命名

> [!warning] 注意
> 回转运动时，是使用当前的 **加速度方向** 决定播放的动画序列，与八向运动时使用 **速度方向** 的做法不同。（最开始没注意，但是似乎影响不是很明显？）
> 
> 主要的原因应该是回转运动过程中，**加速度方向** 是稳定的，而 **速度方向** 中途会变化，变化的枚举值可能造成误解，所以采用了 **加速度方向** 计算的枚举值。
> 
> 那为什么八向运动的时候没考虑用加速度？是因为加速度表示的是未来的趋势，而不是当前的状态，其他的移动状态是稳定的状态，所以使用速度会更准确。

## 5.瞄准偏移

瞄准偏移最好优先使用 `AimOffset` 资产，原因是内部会自动帮忙判断，只能使用 ==网格空间== 的叠加动画。

> [!question] 为什么需要瞄准叠加动画需要使用网格空间？
> 官方文档介绍 [虚幻引擎中的瞄准偏移 | 虚幻引擎 5.5 文档 | Epic Developer Community](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/aim-offset-in-unreal-engine#%E7%BD%91%E6%A0%BC%E4%BD%93%E7%A9%BA%E9%97%B4%E5%8F%A0%E5%8A%A0)
> 
> 选择网格体空间（Mesh Space）而非局部空间（Local Space）的原因在于，选择网格体空间后，可以在骨骼网格体组件（Skeletal Mesh Component）的空间中应用叠加效果。这能确保不管基础动作（Base Pose）的骨骼朝向如何，叠加上瞄准动画后，都会朝向相同的方向。
> 
> ![[UE-Animation-AimOffset-MeshSpaceOrLocalSpace.png|500]]
> 如上图，在左侧的基础动作上，分别叠加上局部空间和网格空间的向上瞄准动画。因为基础动作脊柱已经存在偏移，局部空间下叠加的效果也带上了这里的偏移，导致变成了瞄准斜向上，而网格空间则还是能保证瞄准正上方向。

增加了额外的动画层 FullBody_Aiming，在基础动画的基础上，叠加瞄准动画。

> [!todo] 
> 继续学习了解，动画层的绑定（AnimGraphNodeBinding）具体是有什么用？

对于瞄准动画，比较重要的值有两个：

- AimYaw，水平方向的旋转值，这个就直接等于原地转向时，用于抵消旋转的 `RootYawOffset` 的负值；
- AimPitch，竖直方向上的旋转值，这个值需要使用 `Pawn` 的 `GetBaseAimRotation` 里的 pitch 值；

> [!todo]
> GetBaseAimRotation 的值是来自哪里？

## 6.动画状态机转换问题

在制作过程中，遇到了状态机不符合预期的自动转换问题。

> [!example] 背景
> 在角色播放 **起步动画（Start）** 时，因为只有变为关联时，判断了一次当前前进方向来选择播放的动画。但如果在播放起步动画的过程中，出现了较大幅度的转向，就会出现非常明显的表现异常。比如最开始往左移动，然后马上改为向前移动，因为起步动画的时间也比较长，虽然角色在往前移动，但依然在播放向左移动的动画。
> 
> 为了解决这个问题，这边参考 Lyra 的处理：
> 
> 1. 在 **起步动画** 状态变为关联时，记录当前的角色朝向；
> 2. 增加从 **起步动画** 到 **循环动画（Cycle）** 的过渡，只要朝向不同，就从 **Start** 提前变化到 **Cycle**；
>
> 目前遇到的问题：
> *除了向前起步，其他方向的起步都会直接过渡到Cycle。疑似起步动画变为关联的逻辑还没有触发，就提前执行了过渡的判断？而当前起步方向的默认值时Forward，所以只有向前没事。*

[[动画状态机#1.1.状态连续变化|问题原因]]

解决方法，应该调整动画状态机节点，修改 “每帧的最大过渡” 为1，不允许连续变换状态，确保每个状态内的逻辑都有被执行。

## 7.原地转身

为了实现原地转身，首先比较重要的是必须先实现角色不会直接被相机驱动旋转。为了实现这个效果，需要使用 ”Rotate Root Bone” 节点，给根骨骼设置上一个相反的 Yaw 旋转，来抵消相机带来的 Yaw 旋转，让角色不会一直跟随相机进行旋转。

![[UE-Animation-TurnInPlace-RotateRootBone.png|400]]

为了抵消角色（Pawn）的旋转，需要在更新旋转值的更新函数里，每帧记录当前帧Yaw的旋转角度，并且需要锁定角色旋转时，累加上这里的旋转值，用于最后的抵消。

> [!info] 锁定旋转的状态
> 
> 在 Lyra 中，将需要锁定角色旋转分为了三种状态：
>
> - `BlendOut` 混出，这个状态下，记录的 Yaw 值逐渐减小，最终不再偏移，角色直接朝向相机方向。默认状态，用于角色开始移动之后，希望角色的正方向逐渐变回相机视线方向。
> - `Hold` 保持，维持当前的 Yaw 值不变，用于 Start 状态下，先维持 Yaw 偏移值，避免起步动画过程中切换朝向。
> - `Accumulate` 累加，累加记录 Yaw 值，来抵消旋转值来维持角色朝向，不会直接跟随相机旋转。主要用于 Idle 和 Stop（为了让停止动画提前就开始锁定角色的朝向）。

这里锁定了角色的朝向，角色的朝向与 Pawn 的朝向有一定的偏移，而这里的偏移正好就是 `RootYawOffset`，所以需要改为使用减去了 `RootYawOffset` 的偏转值来计算角色朝向。

---

完成了锁定朝向之后，就需要开始播放转向动画。

> [!info] 转向动画曲线
> 转向动画使用到了两条曲线，在官方提供的修改器中，默认的曲线名分别为：
> - TurnYawWeight 表示转向的权重值，用来表示转向运动是否完成，初始值为1，当值变为0时转向运动完成；
> - RemainingTurnYaw 用于表示起始方向与最后一帧角色方向之间的 Yaw 差值；

转向运动的动画可以分为两个部分：

1. 转向运动中
2. 转向运动后摇

可以根据 TurnYawWeight 的曲线值进行区分，当曲线值为0，表示进入转向后摇。

与回转运动类似，为了支持连续的转向运动，需要对 Idle 状态进行调整，内部再使用动画状态机，可以重复进入转向运动。

![[UE-Animation-TurnInPlace-StateMachine.png|500]]

> 只要 `RootYawOffset` 超过阈值（按照Lyra的写法，这里是写死了50°），就进入 TurnInPlaceRotation 的原地转身状态，转身运动完成后，继续进入转身动画的后摇 TurnInPlaceRecovery，在这个状态，不会播放 Idle 的闲置动画，如果过程中继续转身，可以再次进入 TurnInPlaceRotation 继续播放转身动画。

## 8.跳跃着陆动画

首先，对跳跃状态进行分析，可以将其分为以下几个大的阶段：

- 上升阶段（JumpStart）
- 上升最高点（JumpApex）
- 下落循环（FallLoop）
- 落地阶段（FallLand）

![[UE-Animation-JumpAnimGraph.png|500]]

从其他运动状态转换到跳跃状态有两个主要的入口：

- JumpStart，起跳状态，带有上升速度（z轴方向上的速度为正值）的状态下进入跳跃状态机，播放起跳动画，并且在到达最高点之前，循环播放跳跃循环动画（JumpStartLoop）；
- JumpApex，跳跃最高点，没有上升速度直接进入跳跃状态，通常是从高处掉落；

在到达最高点之后，进入掉落阶段，在距离地面较远时，播放掉落循环动画（FallLoop），接近地面时，开始播放落地动画（FallLand）。利用状态别名JumpFallInterruptSources，支持了其中的任意状态，如果提前落地，就退出跳跃状态（比如往上跳跃的过程中上台阶，根本没有走到下落的流程）。

跳跃状态的出口（落地之后的状态）也有两个：

 - 如果有加速度，直接进入 Cycle 开始播放移动循环动画；
 - 如果没有加速度，直接进入 Idle 待机动画；

跳跃这些状态中，只有 FallLand 状态比较特殊，为了动画与下落的距离匹配，与 Stop 类似，需要使用到 `Distance Match To Target` 结点匹配距离。

> [!info] 补充
> 
> 在Lyra示例项目中，这里的地面距离计算是放在了 `ULyraCharacterMovementComponent::GetGroundInfo` 中，这个函数是在 `ULyraAnimInstance` 的 `NativeUpdateAnimation` 中每帧更新。
> 
> 这里的地面距离是通过射线检测 `LineTraceSingleByChannel` 计算，需要注意的小细节是，通常情况下，使用的起始点 `GetActorLocation` 不在角色的脚底，而是角色正中心，所以需要减去胶囊体半高。


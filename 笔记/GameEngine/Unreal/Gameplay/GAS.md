GAS，Gameplay Ability System，是一个见状的，高度可扩展的GamePlay矿建。常用于角色扮演（RPG）、动作冒险等常见游戏的战斗逻辑框架。通过GAS，可以快速地制作游戏中地主动/被动技能，各种效果buff、计算属性伤害、处理玩家各种战斗状态逻辑。

提供了功能（部分实例，实际还能扩展出其他更复杂的效果，这里只是展示几种最常见，可以快速制作的功能）：

1. 实现了带有消耗和冷却的角色技能；
2. 处理数值属性（生命、魔法、攻击、防御）
3. 应用状态效果（着火、击飞、眩晕）
4. 应用游戏标签（GameplayTags）
5. 生成特效和音效
6. 完整的网络复制、预测功能

# 功能概述

GAS的功能划分

## ASC

ASC，Ability System Component，是GAS的基础组件。ASC本质上是一个 `UActorComponent`，用于处理整个框架的交互逻辑，包括使用技能（GameplayAbility）、包含属性（AttributeSet）、处理各种效果（GameplayEffect）。

所有需要应用GAS的对象（Actor），都需要拥有GAS组件。用于ASC的Actor被称为ASC的 **OwnerActor**，ACS实际作用的Actor被叫做 **AvatarActor**。ASC可以被赋予某个角色ASC，也可以被赋予PlayerState（可以保存死亡角色的一些数据）。

## Gameplay Ability，GA

GA，Gamplay Ability，标记了游戏中一个对象Actor可以执行的行为或技能。

能力（Ability）可以是普通攻击或者吟唱技能，可以是角色被击飞倒地，还可以是使用某种道具，交互某个物件，甚至跳跃、飞行等角色行为，都可以算作Ability。

Ability可以被赋予对象或者从对象的ASC中动态移除，对象可以同时激活多个GameplayAbility。

> 但基础的移动输入、UI交互行为则不能或不建议通过GA来实现
> 为什么？可能是性能？

![[UE-GAS-GA-Flowchart.png]]

## Gameplay Effect，GE

GE，Gameplay Effect，是Ability对自己或其他人产生影响的途径。

GE通常可以理解为游戏中的buff，包括增益/减益效果（修改属性），GAS中的GE更加广义，释放技能时的伤害结算，释放特殊效果的控制、霸体效果（修改GameplayTag）都是通过GE来实现的。

GE相当于可配置的数据表，不可添加逻辑。开发者创建一个 `UGameplayEffect` 的派生蓝图，就可以根据需求制作想要的效果。

## Gameplay Cue Notify，GC

GC，Gameplay Cue Notify

## Gameplay Tags，Tag

FGamePlayTags 是一种层级标签，如 Parent.Child.GrandChild。通过GameplayTagManager进行注册。

替代了原本的Bool或Enum的结构，可以在玩法涉及中更高效的标记对象的行为或状态。

## Attribute Set，Attribute

Attribute Set，负责定义和持有属性，并且管理属性的变化，包括了网络同步。

需要在Actor中被添加为成员变量，并注册到ASC（C++），一个ASC可以拥有一个或多个（不同的AttributeSet，因此可以角色共享一个很大的AttributeSet，也可以每个角色按需添加AttributeSet。

可以在属性变化前（`PreAttributeChange`）和后（`PostGameplayEffectExecute`）处理相关逻辑，可以通过委托的方式绑定属性变化。


通过继承实现 `FGameplayAttribute`，来定义自己的属性，其中的成员使用 `FGameplayAttributeData` 来表示存储属性值，在 `FGameplayAttributeData` 中保存有两条float值，一个 `BaseValue` 和 `CurrentValue`，额外存储了一个 `BaseValue` 主要是方便用来计算能力变化，以及变化结束后的回滚。

## Gameplay Task，Task

## Gameplay Event Data，Event

# 类型功能简介

谁能释放技能？AbilitySystemComponent
技能的逻辑？GameplayAbility
技能的效果？GameplayEffect
技能改变的属性？GameplayAttribute
技能涉及的条件？GameplayTag
技能的视效？GameplayCue
技能的长时行动（延迟异步或持续时间的逻辑）？GameplayTask
技能消息事件？GameplayEvent

# 系统结构

![[UE-GAS-Framework-UML.png]]

# 流程

> [!info] Lyra中一次涉及的流程
> 参考 [【UE5 C++ GAS】虚幻引擎5 手把手带你看GAS Lyra一次完整攻击流程](https://www.bilibili.com/list/watchlater?bvid=BV1H7cLe6EN4&oid=113799705135779)

```plantuml
start
:Send GameplayEvent to Actor;
note right
通过tag（GameplayEventData）找到合适的Gameplay Ability
核心是UAbilitySystemComponent::HandleGameplayEvent
end note
:TriggerAbilityFromGameplayEvent;
note right
触发对应tag的所有Gameplay Ability
end note
partition "Gameplay Ability 流程" {
:调用GA的PreActivate接口;
:调用GA的ActivateAbility接口;
partition "Lyra自定义的GA逻辑" {
:ULyraGameplayAbility_RangedWeapon::StartRangedWeaponTargeting;
note right
大致的流程是做了射线检测，找到攻击目标
end note
if (找到攻击目标) then (true)
:OnRangedWeaponTargetDataReady（由蓝图实现）;
:ApplyGameplayEffectToTarget;
note right
<color:red>重要接口</color>，应用GE
这里的核心函数 UAbilitySystemComponent::ApplyGameplayEffectSpecToSelf、
FActiveGameplayEffectsContainer::ApplyGameplayEffectSpec
end note
:UAbilitySystemTestAttributeSet::PostGameplayEffectExecute;
note right
触发AttributeSet的数据修改
end note
if (生命值不足) then (true)
:通过OnOutHealth委托，触发\nULyraHealthComponent::HandleOutOfHealth;
:使用HandleGameplayEvent播放死亡事件;
note right
与上面类似，最后触发ULyraGameplayAbility_Death
end note
endif
endif
}
}
end
```

# 实现分析

## Gameplay Task

实现自定义 GT 的方法：

1. 需要先定义动态多播委托类；
2. 需要增加对应委托类型的成员变量，并且设置为 `BlueprintAssignable`，来暴露成输入引脚；

```cpp
// 1. 定义委托类，委托类的参数将被作为输出返回
// 第一个参数表示参数名
// 后面的两两一组，表示参数类和参数名
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);
// 以上面的代码为例，在最后的节点上，就会额外出现两个引脚EventTag和EventData作为输出

// 2. 继承实现自定义的 GameplayTask 类
// 此处使用的是 ActionRPG 官方示例中的代码，逻辑与 UAbilityTask_PlayMontageAndWait 基本一致，最大的区别是委托额外返回了两个参数
UCLASS()
class URPGAbilityTask_PlayMontageAndWaitForEvent : public UAbilityTask  // UAbilityTask是UGameplayTask的子类
{
	GENERATED_BODY()
	
public:

	// 几个 UGameplayTask 的重要接口
	
	// 核心接口，实现自定义的异步逻辑，在不同的状态下触发对应的委托
	// 开始播放蒙太奇动画，并且绑定了播放完成 OnCompleted、混出 OnBlendOut、取消 OnCancelled、打断 OnInterrupted 的回调
	// 相比通用的 UAbilityTask_PlayMontageAndWait，ActionRPG 这里还有一个 EventReceived 事件，用于ASC触发对应的 EventTag 时调用
	virtual void Activate() override;
	// 取消，触发 OnCancelled
	virtual void ExternalCancel() override;

	/** The montage completely finished playing */
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnCompleted;
	
	/** The montage started blending out */
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnBlendOut;
	
	/** The montage was interrupted */
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnInterrupted;
	
	/** The ability task was explicitly cancelled by another ability */
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnCancelled;
	
	/** One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate EventReceived;
}
```

![[UE-GAS-GameplayTask-Example.png|300]]

如上图，上面添加的成员 `OnCompleted`、`OnBlendOut`、`OnInterrupted`、`OnCancelled`、`EventReceived` 都有对应的流程引脚，而委托类的两个参数 `EventTag` 和 `EventData` 也出现在了输出引脚的最后。


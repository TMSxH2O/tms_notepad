
学习 Lyra 关于 Gameplay 设计的思考

Lyra 中大量使用了数据驱动的设计，将所有的选择都调整为配置的方式，而不会出现在代码或是蓝图中写死，导致逻辑之间发生耦合。

# 绑定输入

这些配置发生在每个关卡设置的 `LyraExperienceDefinition` （纯数据类）中，`LyraExperienceDefinition` 的解析逻辑则是发生在 `ULyraExperienceManagerComponent`，而管理组件则是注册在了 `GameState` 中。

其中的按键输入、默认组件、界面HUD，通过 `ActionSet` （`UGameFeatureAction`）的方式，进行了绑定。其中可以分为两种类型，基础输入（比如移动，旋转视角等），由于是非常常用的流程，是直接实现在了 `ULyraHeroComponent` 中（比如 `Input_Move` 函数）。在 Lyra 示例中，`InputMappingContext` 是通过 GameFeatureData 使用 Add Input Mapping 进行绑定的。在 `LyraInputConfig` 里只考虑 `InputAction` 对应的 `InputTag`。

输入与ASC进行联动，输入激活的GameplayTag，是在每帧更新的时候（`Controller::PostProcessInput`），根据GameplayTag去触发能力。

![[UE-Input-Initialize.png]]

## Gameplay Cue

Gameplay Cue 在 `LyraExperienceDefinition` 里，是通过 `UGameFeatureAction_AddGameplayCuePath` 记录了路径，最后通过 `UAbilitySystemGlobals::AddGameplayCueNotifyPath` 添加到管理类。最后是在 `UGameplayCueManager::InitObjectLibrary` 里通过路径加载出 `UGameplayCueNotify_[Static|Actor]` 对象。

## Gameplay Ability

GA 是通过 `ULyraAbilitySet` 进行配置。所有的 GA 与输入之间，则是通过 `InputTag` （FGameplayTag类）进行关联。

在 Lyra 中，主要有两个部分来绑定 `ULyraAbilitySet`（目前只看到两个部分）：

- 在 `ULyraPawnData` 中，会设置默认的 `AbilitySet`，仅包括了通用的能力；
- 在 `ULyraEquipmentDefinition` 中，与武器绑定，获取武器时获取到对应的 `AbilitySet`；

# 装备拾取

通过 `ALyraWeaponSpawner` 实现，内部的核心是脚本实现的 `GiveWeapon` 函数，用来把武器赋予Pawn 

1. 实现 `ALyraWeaponSpawner` 类，用来呈现一个获取装备的 Actor 对象，用于场景中显示；
2. 实现 `PickupDefinition` 类，保存可拾取物信息，内部关联 WeaponDefinition 保存武器信息（`WeaponPickupDefinition`），武器信息里保存了可以随意扩展的 `InventoryItemDefinition`（内部是通过模板 `InventoryItemFragment` 的方式，存储了不同类型的数据）；
3. 这里再继承扩展 `InventoryItemFragment` 里面最重要的是 `UInventoryFragment_EquippableItem` 来保存了装备后的信息 `EquipmentDefinition`
4. 装备信息里的三条数据都很重要：`ULyraEquipmentInstance` 装备实例信息，保存了装备的大部分具体配置，并且用来实现装备武器和卸载武器时的逻辑（切换连接对应武器的动画蓝图）；武器携带的 `AbilitySet`（装备的 GA、GE 和 AS）；Spawn武器所需的信息（创建的 Actor 类型、挂载点、挂载偏移信息）；

拾取装备到赋予能力的堆栈如下：
![[UE-Lyra-Pickup-GiveAbility.png]]

这里值得注意的是，`GiveAbility` 时，已经把装备实例 `ULyraEquipmentInstance` 保存到了 `GameplayAbilitySpec` 的 `SourceObject`，用于武器能力 `ULyraGameplayAbility_FromEquipment` 也可以获取武器信息


# 武器射击

武器的射击是通过 `LyraGameplayAbility_RangedWeapon` 里实现的。

- `StartRangedWeaponTargeting` 来进行射线检测，获取武器命中的对象数据（`FGameplayAbilityTargetDataHandle`）；
- `OnRangedWeaponTargetDataReady` 用于处理命中目标的逻辑；

在 Lyra 的射击开火流程中，GE 只用来修改目标的血量，GCN 是在 GA 里通过 Tag 触发。其中，GE是比较常规的给 `HealthSet` （AttributeSet） 修改血量（先修改 Damage 字段，然后间接造成伤害）。这里的 GCN 相对比较复杂，展开分析：

在 `GCN` 内部，并没有直接处理视觉效果，而是调用了 `B_Weapon` 的蓝图类（应该是还不稳定，不然后续肯定会调整成C++代码）。
在 `B_Weapon` 的 `Fire` 事件中，处理了三项内容：枪口火焰子弹特效、命中点弹孔贴花、命中点烟雾特效。

现在的简单处理只支持枪口火焰和子弹特效。

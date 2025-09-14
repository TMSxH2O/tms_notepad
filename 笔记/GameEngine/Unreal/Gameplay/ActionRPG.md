
在 UE5 中尝试复现 ActionRPG 官方示例。

目标是学习 GAS，ARPG 类型游戏基础。

# GameInstance

目前看下来，GameInstance是用来控制游戏初始状态（比如打开开始菜单等）以及网络连接用的，对目前想学习的内容关系不大，所以暂时先跳过。

# 角色状态初始化

角色加载到场景后，会自动绑定相关的武器和法术，后续直接通过tag来触发已经激活的 GameplayAbility。

> [!question] Ability System Component 组件是放在了 ACharacter 中，而不是 Controller 中，这是为什么？
> 暂时没有找到太多资料，比较常见的说法是 `ASC` 是一个与表现更加相关的内容，因此不适合放在 `Controller` 中（？）。而且经常与 `AttributeSet` 这种角色数据会一起出现，`Controller` 应该只关注角色控制相关的内容。
> 
> 在Lyra里，官方是将ASC组件放在了 `APlayerState` 上（Model层），感觉更加合理一些。

武器Actor的初始化流程如下：

```plantuml
start
:RPGPlayer Controller Base::OnProcess;
note right
控制器的初始化流程
<color:red>每个Controller都会通过Process接口关联到Pawn，可以作为初始化的接口使用</color>
end note
:RPGCharacter Base::CreateAllWeapons;
note right
角色创建关联的所有武器
end note
partition "创建所有武器" {
:获取物理类型;
note right
保存在Controller的SlottedItems
end note
:遍历创建对应的WeaponActor实例;
:触发Character的WeaponAttachMethod;
note right
将当前武器移动到虚拟点
end note
}
end
```

> [!info] Controller 重要流程
> 每个 `AController` 都需要通过 `Process` 接口关联控制一个 `APawn`。这里有两个比较重要的回调接口，会经常被使用：
> - `AController::OnProcess` 控制器关联 `ACharacter` 时调用，按照官方的用法，可以视作 `AController` 的初始化接口；
> - `ACharacter::ProcessedBy` `ACharacter` 被关联到 `AController` 时调用，发生在 `AController::OnProcess` 之后，可以用于 `ACharacter` 中实现部分与 `AController` 相关的初始化逻辑；

Gameplay Ability 初始化：

```plantuml
start
:Character::ProcessBy;
note right
角色的初始化
end note
:Character::AddStartupGameplayAbilities;
note right
添加默认的Gameplay Ability
end note
:Character::AddSlottedGameplayAbilities;
note right
将默认的SlottedAbilities和
Controller中的SlottedItems
都使用GiveAbility添加到了
GAS组件中
end note
end
```

# 制作记录

## 输入绑定

Lyra支持了用户自定义的按键映射

需要额外补充 `InputModifier`，从用户的配置上读取信息，来覆盖按键设置。

## 支持GameState和GameMode

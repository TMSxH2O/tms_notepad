
#学习笔记 #UE架构

# 1.Actor和Component

使用Actor，是用来表示对象之间的继承关系。它自己并不包含表示场景坐标的能力，而是通过 `RootComponent` 来达成。

> 这说法是不是有点牵强，那照这样的说法，其实Unity的GameObject也没有表示变换的能力，而是通过固定创建的 `TransformComponent` 来完成

引入组件，通过聚合的方式，为 Actor 扩展各种能力。

```plantuml
UClass <|-- UObject
UObject <|-- AActor
UObject <|-- UActorComponent
UActorComponent <|.. USceneComponent
USceneComponent <|.. UChildActorComponent

AActor *-- UActorComponent
```

# 2.Level和World

每个游戏世界（World）里，可能存在多个场景（Level），由每个 Level 来管理其中的 Actor。

```plantuml
ULevel o-- AActor
AActor <|-- ALevelScriptActor
AActor <|-- AInfo
AInfo <|-- AWorldSettings

ULevel o-- ALevelScriptActor
ULevel o-- AWorldSettings
```

# 3.WorldContext，GameInstance，Engine

游戏中不止一个World（比如编辑器中，编辑预览场景、PIE场景、角色编辑器显示的预览场景），`FWorldContext` 则是用来存储视口切换显示的 World 信息。

```plantuml
class FWorldContext
{
OwningGameInstance: UGameInstance*
ThisCurrentWorld: UWorld*
GameViewport: UGameViewportClient*
}
class UWorld
{
+ Levels: TArray<ULevel*>
+ GameState: AGameState*
+ WorldComposition: UWorldComposition*
+ PhysicScene: FPhysScene*
- PersistentLevel: ULevel*
- CurrentLevel : ULevel*
- StreamingLevels : TArray<ULevelStreaming*>
- OwningGameInstance: UGameInstance*
}
FWorldContext o- UWorld
```

WorldContext用来保存World的信息，它自己则是保存在 GameInstance 中

```plantuml
class UGameInstance
{
WorldContext: FWorldContext*
LocalPlayers: TArray<ULocalPlayer*>
OnlineSession: UOnlineSession
}
UGameInstance o- FWorldContext
```

继续往上，找到管理 GameInstance 的类，Engine 类

```plantuml
circle GEngine
note left
每次启动，创建的UEngine全局对象
end note

class UEngine
{
WorldList: TIndirectArray<FWorldContext>
}

class UEditorEngine
{
PlayWorld: UWorld*
EditorWorld: UWorld*
AllViewportClients: TArray<FEditorViewportClient*>
}
class UGameEngine

GEngine - UEngine
UEngine <|-- UGameEngine
UEngine <|-- UEditorEngine
UGameEngine o-- UGameInstance
UEditorEngine o-- UGameInstance
```

# 4.Pawn、Controller、PlayerState

`APawn`，通常代表游戏中 *可控制的角色* 或 *AI驱动的对象*。`Pawn` 类本身不具备控制逻辑，它提供了基础的框架，用来表示游戏中的 *可移动实体*，玩家或AI通过控制器间接取操控 `Pawn`。

> [!question] 与Actor的区别
> 虽然 `AActor` 也有 `InputComponent` 可以接收输入，但 `APawn` 提供了更多与控制器和移动（运动、碰撞和物理模拟）相关的功能，适用于表示可由玩家或AI控制的对象。

`AController` 控制器，是玩家或AI控制的桥梁，接受输入并将其转化为 `Pawn` 的行为。在多人游戏中，它还负责同步其他玩家的操作与状态，确保每个客户端上的 `Pawn` 能够正确反映其他玩家的输入。

`APlayerState` 保存的是每个角色的状态信息。

到这里，每个角色的三个重要的类就都全部出现，以 MVC（Model-View-Controller）框架进行实现。Model，模型，`APlayerState`；View，视图，`APawn`；Controller，控制器，`AController`。

```plantuml
class AActor
class APawn extends AActor
class AController extends AActor
class AInfo extends AActor
class APlayerState extends AInfo
class APlayerController extends AController
class AAIController extends AController
APawn <- AController : 控制
AController *- APlayerState : 数据
```

# 5.GameMode和GameState

`AGameModeBase`（在 4.14 版本之前的基类是 `AGameMode`）

- 游戏内的实体Spawn<br>登记游戏中需要的类型信息，在进入场景后自动 Spawn 出对应的对象添加到场景中。
- 游戏进度<br>控制一个游戏支不支持暂停，如何重启等涉及到游戏内状态的操作。提供了 `SetPause`、`RestartPlayer` 等函数来实现常见流程逻辑。
- Level的切换<br>GameMode决定了刚进入一场游戏的时候，是否应该播放开场动画，也决定了当决定了要切换到下一个关卡时，是否需要 `bUseSeamlessTravel`，一旦开启，就可以重载GameMode和PlayerController的 `GetSeamlessTravelActorList` 和 `GetSeamlessTravelActorList` 函数来指定哪些Actors可以进入下一个World。
- 多人游戏的步调同步<br>用来实现常见的等待所有玩家准备并加载完成之后，才开始游戏的功能。

`AGameState` 负责保存当前游戏的状态，比如任务数据、当前对战的计分板等。

> [!question] 多个Level配置不同GameMode时会使用哪个GameMode？
> 同一个World内，只会有一个GameMode，而一个World下可以有多个Level。这种情况，只会使用 `PersistenLevel` 的配置，后续加载的 `SubLevels`（StreamingLevel），因为此时已经存在GameMode，就不会再创建出其他的GameMode了。

> [!question] 哪些逻辑应该写在GameMode里？哪些应该写在Level Blueprint里？
> - Level Blueprint主要控制Level内表现（Level内的局部规则）；GameMode用来控制World内全局的规则<br>由于World只有一个GameMode，而每个Level都有自己的Level Blueprint（也就是说一个World可能有多个不同的Level Blueprint）。
> - GameMode只在 Server 存在，因此GameMode应该注意不能写和具体Client（表现相关）的逻辑，比如控制UI等。

```plantuml
UWorld *-- ULevel
ULevel *- AGameMode
UWorld o-- AGameMode : 控制
AGameMode *- AGameState : 数据
class AGameMode extends AInfo
class AGameState extends AInfo
```

# 6.Player

大致分为本地玩家和网络链接玩家两种：

本地玩家（`ULocalPlayer`），直接从 `UPlayer` 类派生的子类

```plantuml
class ULocalPlayer extends UPlayer
UGameInstance o- ULocalPlayer
```

网络链接玩家（`UNetConnection`)，在UE里，是将一个网络链接视作一个Player

```plantuml
class UNetConnection extends UPlayer
UGameInstace o-- FWorldContext
FWorldContext o-- UNetDriver
UNetDriver o- UNetConnection

class IpConnection extends UNetConnection
class UChildConnection extends UNetConnection
```

# 7.GameInstance

`UGameInstance` 一个引擎全局的对象（准确点说是每个 Engine）对象中有一份。可以用来管理一些全局的对象（比如上面的 Player）。此外，它还提供了以下几类接口：

1. 引擎的初始化加载，`Init` 和 `ShutDown` 等
2. Player 的创建和管理，如 `CreateLocalPlayer`、`GetLocalPlayer` 等
3. GameMode的重载修改。从 4.14 开始支持通过 GameInstance 靠重载 `PreloadContentForURL`、`CreateGameModeForURL` 和 `OverrideGameModeClass` 方法，来修改GameMode。
4. OnlineSession的管理，管理网络会话？

# 总结

UE 的类在设计上，一直采用 MVC 架构的方式进行设计，每个功能都有一组类相互对应。分别用来实现 `Model` 数据、`View` 呈现、`Control` 控制（逻辑）三个部分。

![[InsideUE4-总结图.png|400x]]
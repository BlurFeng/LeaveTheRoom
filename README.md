# 【LeaveTheRoom】
***阅读中文文档 >[中文](README.md)***\
***日本語のドキュメントを読む >[日本語](README_ja.md)***\
***Read this document in >[English](README_en.md)***

![](Documents/LeaveTheRoom_1.png)
![](Documents/LeaveTheRoom_2.png)

## 【项目介绍】
使用UnrealEngine5.4.4版本开发的展示用游戏项目。\
玩家扮演一只猫，在多个房间之间通过“传送门”穿梭，寻找“鱼”来获得积分，并在限定时间内通过“离开门”来离开房间获得胜利！\

## 【操作方式】
WSAD或方向键：移动\
鼠标移动：旋转镜头\
鼠标单击：对准鱼后单击获得积分\
鼠标长按：对准“传送门”或“离开门”长按使用门\
空格键：跳跃\
按住LeftShift：慢速移动

## 【项目开发内容】
使用UnrealEngine5.4.4版本开发，基于Gameplay框架开发业务功能。使用C++编辑底层框架代码和功能插件，使用蓝图实现业务层逻辑开发。

### C++
1. 项目框架底层构建自己的GameActor等功能性类，用于在上层业务和UnrealEngine引擎之间再做一次隔离。便于后期扩展和维护。
2. DetectionSystem：探测系统，基于EQS开发。用于在环境内搜索需要的目标。
3. BlurFengLibrary：等项目通用数学库和方法库。

### 项目资产及蓝图
#### 插件
1. DetectionSystem：使用蓝图提供了Debug用的UMG。以及一个单独的插件功能展示用Demo场景。
#### 项目内容
1. 基于UE的Gameplay框架，将业务逻辑分离在GameMode、GameState、PlayerController、Character。
2. 使用全新的EnhancedInput来制作玩家操作输入的功能。
3. 制作根据移动状态变化的角色动画状态机。
4. 玩法内容核心类InteractionActor基类，及其子类TeleportDoor、ExitDoor和ScoreItem。
5. 根据业务需求制作DetectionSystem使用的配置表和EQS资产。
6. 房间内容根据配置表自动化生成。包括TeleportDoor、ExitDoor、ScoreItem和AirWallBox。通过配置表将业务功能可配置化，易于业务内容的生产。
7. UI：使用UMG搭建项目的主界面UI及操作提示UI。以及UI用的Material的开发。

## 【其他】
展示用项目，实际开发时间十天。
游戏资源来自UE商店。

## 【TODO】
1. 房间配置目前只有底下的13个房间。
2. 房间配置表可视化配置工具待制作。
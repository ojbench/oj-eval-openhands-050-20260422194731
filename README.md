# Problem 050 - 恶魔轮盘

**ACMOJ Problem ID**: 3028

## Table of Contents

- [Problem 050 - 恶魔轮盘](#problem-050-恶魔轮盘)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Background](#background)
  - [Assignment Description](#assignment-description)
    - [Grade Composition](#grade-composition)
  - [Assignment Requirements](#assignment-requirements)
    - [Input Format](#input-format)
    - [Output Format](#output-format)
    - [Samples](#samples)
    - [Data Constraints](#data-constraints)
  - [Per-Testcase Resource Limits](#per-testcase-resource-limits)
  - [Test Data](#test-data)
  - [Submission Requirements](#submission-requirements)
    - [Submission Format](#submission-format)
    - [Submission Guidelines](#submission-guidelines)
    - [Evaluation Notes](#evaluation-notes)
    - [Academic Integrity](#academic-integrity)

## Introduction

### Background

Implement game state class for Buckshot Roulette: simulate game mechanics, probability calculation, item system.

## Assignment Description

### Grade Composition

| Grading Component | Percentage |
| :--: | :--: |
| Pass **3028. 恶魔轮盘** | 80% |
| Code Review | 20% |

Here are several points that need clarification:

- In the Code Review, we will **strictly examine your code style and repository organization structure, etc.**

- This assignment provides some sample data for testing, stored in the `/workspace/data/050/data_test/` directory. Note that these are not the test cases on the Online Judge. Passing all local test cases does not guarantee that you will pass the OJ tests.

- Besides the provided sample data, we also encourage you to design your own test data based on your program logic to assist debugging.

## Assignment Requirements

### Problem Description

在 ECF 的赛场上，小猪被恶魔轮盘题阴了一手，并且失去了冠军。

## 题目目标

你需要实现一个游戏状态类 `GameState`，用于模拟恶魔轮盘对局。

- 评测端提供 `main.cpp`（负责读输入、调接口、输出）。
- 你只需要实现 `game_state.hpp`。

需要实现的内容：

1. 模拟游戏操作接口：处理 6 种 `op` 并正确更新状态。
2. 查询下一颗子弹接口：查询下一发是 `live/blank` 的概率。
3. 判断游戏是否结束接口：若未结束返回 `-1`，否则返回胜者 id（`0/1`）。

以下是恶魔轮盘游戏的规则：

## 规则简述

### 1. 基础状态

- 游戏由一把手枪和两名玩家组成，玩家 `0` 先手，玩家 `1` 后手，双方初始生命值均为 `5`。在游戏的一开始，手枪会装填 `6` 颗子弹，并且每个人会有两个对应的道具。玩家会不断的进行操作，直到有人死亡（生命值变为 0）

- 子弹分为 `live`（实弹）与 `blank`（空包弹），只知道数量，不知道顺序，所有顺序都是可能的。

- 每名玩家最多持有 `2` 个道具。

### 2. 回合与射击

- 当前行动方在自己回合内可以连续执行操作，在结束自己的回合后，会开始对方的回合。
- 回合通常在“某次射击导致换手”时结束；若有人生命值降到 `0` 或以下，游戏立即结束。
- `fireAtOpponent`：朝对手开枪。若为实弹，对手 `HP-1`。无论子弹的结果如何，都会结束自己的回合。
- `fireAtSelf`：朝自己开枪。若为实弹，自己 `HP-1` 且结束自己的回合；若为空包弹则继续自己的回合。
- 特殊情况：结束自己的回合的时候，如果玩家使用了 `cage` 道具并且仍然有效（见下文），则抵消一次结束回合（即继续自己的回合）。

### 3. 道具效果

- `useDummy`：消耗下一发子弹，所有玩家都会知道消耗的子弹类型。不结束自己的回合。
- `useMagnifier`：所有的玩家都会获知下一发子弹类型，不结束自己的回合。
- `useConverter`：所有的玩家都会获知下一发子弹类型，并且将下一发 `live -> blank`，`blank -> live`（即翻转子弹类型），不结束自己的回合。
- `useCage`：获得一个一次性效果：在自己射击后需要结束回合的时候，抵消一次结束回合（即继续自己的回合）。该效果在触发后立即失效；同一回合内至多使用一次 `use_cage`（即使持有多个 `cage` 并且之前 `cage` 的效果失效了也不允许）。

### 4. 子弹与道具补充

- 当游戏刚开始的时候，或者当子弹被消耗到 `0` 时，需要进入补给阶段，并且要**同时完成子弹补充和道具补充**，你需要完成补充子弹和道具的接口。
- 在输入指令中，这对应为：
- `supply bullet l b`（补充子弹）
- `supply item id name`（补充道具）
- 也就是说，子弹打空后，不仅要补子弹，也要补道具，然后再继续后续操作。

## 你需要实现的接口（`game_state.hpp`）

```cpp
#include <exception>

class InvalidOperation : public std::exception {
public:
    const char* what() const noexcept override {
        return "invalid operation";
    }
};

// 记录玩家的装备信息
struct PlayInfo {
    int dummyCount = 0;
    int magnifierCount = 0;
    int converterCount = 0;
    int cageCount = 0;
};

class GameState {
public:
    enum class BulletType {Live, Blank};
    enum class ItemType { Dummy, Magnifier, Converter, Cage };

    // 初始构造函数
    GameState();

    // 六个操作（除 useCage 外都带“操作前顶端子弹信息”）
    void fireAtOpponent(BulletType topBulletBeforeAction);
    void fireAtSelf(BulletType topBulletBeforeAction);
    void useDummy(BulletType topBulletBeforeUse);
    void useMagnifier(BulletType topBulletBeforeUse);
    void useConverter(BulletType topBulletBeforeUse);
    void useCage();

    // 补充接口
    void reloadBullets(int liveCount, int blankCount);
    void reloadItem(int playerId, ItemType item); // playerId: 0/1

    // 查询接口
    double nextLiveBulletProbability() const;
    double nextBlankBulletProbability() const;

    // 终局判断：未结束返回 -1，结束返回胜者 id（0/1）
    int winnerId() const;
};
```

## 异常约定（实现要求）

在接口实现中，若遇到非法操作，必须 `throw` 一个异常对象 `InvalidOperation`。

本题保证只会出现以下两类非法操作：

1. 使用道具时，该玩家并没有该道具。
2. 同一回合内重复使用 `use_cage`（即使持有多个 `cage` 也不允许）。

不需要处理其他不符合游戏规则的情况，数据保证不会出现除了以上两种异常以外的异常。

异常处理要求：

- 抛出异常的这次 `op` 视为未发生，状态不改变。
- 评测端 `main.cpp` 会捕获该异常并输出 `error!`。

## 部分分设计

| 子任务       | 分值  | 操作指令数上限 | 特殊性质                                                                                                   |
| --------- | --- | ------- | ------------------------------------------------------------------------------------------------------ |
| Subtask 1 | 20% | `10^5`  | 只会出现 `fire_opponent`、`fire_self` 两种操作；不会出现非法操作。                                                        |
| Subtask 2 | 20% | `10^5`  | 只会出现 `fire_opponent`、`fire_self`、`use_dummy`、`use_magnifier`、`use_converter`；不会出现 `use_cage`；不会出现非法操作。 |
| Subtask 3 | 40% | `10^5`  | 六种操作都可能出现；不会出现非法操作。                                                                                    |
| Subtask 4 | 20% | `10^5`  | 无特殊性质（完整数据）。                                                                                           |

---

## 以下是辅助检查与调试信息（非实现要求）

你可以参考下发文件中的 `main.cpp` 与你写的 `game_state.hpp` 一起编译，检查和调试。

### Input Format

第一行一个整数 `T`，表示有 `T` 局游戏。

每局内持续读取指令，直到该局出现胜者。指令分三类：

- `op <name> [bullet]`
- `query <live|blank>`
- `supply ...`

### 1. 操作指令 `op`

`name` 取值：

- `fire_opponent`
- `fire_self`
- `use_dummy`
- `use_magnifier`
- `use_converter`
- `use_cage`

说明：

- 若 `name != use_cage`，还会提供 `bullet`，取值 `live/blank`。
- 该 `bullet` 表示“本次操作前顶端子弹类型”。

### 2. 查询指令 `query`

- `query live`：查询下一发是实弹的概率。
- `query blank`：查询下一发是空包弹的概率。

### 3. 补充指令 `supply`

- `supply bullet l b`：补充子弹（`l` 发实弹，`b` 发空包弹）。
- `supply item id name`：给玩家 `id` 补充一个道具。
  - `id` 为 `0/1`
  - `name` 为 `dummy/magnifier/converter/cage`

### Output Format

- 遇到 `query`：输出对应概率，保留两位小数。
- 某局结束：输出 `player 0 win!` 或 `player 1 win!`。
- 若 `op` 非法（接口抛异常）：输出 `error!`，并且该操作视为未发生（状态不变）。

### Samples

No sample data provided for this problem.

### Data Constraints

See the problem description for constraints.

## Per-Testcase Resource Limits

- **Time Limit (per test case)**: 1000 ms
- **Memory Limit (per test case)**: 512 MiB
- **Disk Usage**: No disk usage is permitted.

## Test Data

The test data for this problem is located at `/workspace/data/050/data_test/`.

Attachments and starter files for this problem are located at `/workspace/data/050/`.

## Submission Requirements

### Submission Format

Submit a single C++ source file to the Online Judge. The OJ provides its own `main.cpp` which `#include "src.hpp"`. Your submission will be written to `src.hpp` and compiled together with the OJ's driver code.

### Submission Guidelines

- The submitted code must be able to compile successfully through the above compilation process
- The program needs to be able to read data from standard input and write results to standard output
- Please ensure the code runs correctly within the given time and space limits
- **You must use C++ or C language** to implement this assignment

### Evaluation Notes

- The evaluation system will test your program using the provided test data
- The program output must exactly match the expected output (including format)
- Exceeding time or memory limits will be judged as the corresponding error type
- Please pay attention to the overall time performance of your code and the time complexity of each part of your algorithm.

### Academic Integrity

If any violations are found during evaluation or code review (including but not limited to using unconventional methods to pass test cases), your final score may be significantly reduced or become **0 points**.

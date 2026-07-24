# Dayan Yarrow-Stalk Divination
# 大衍蓍草卜筮模拟程序

A C++ simulation of the traditional Dayan yarrow-stalk divination process, with hardware-generated randomness, individual stalk tracking, hexagram mapping, changing-line calculation, and command-line random-number generation.

一个使用 C++ 编写的大衍蓍草卜筮模拟程序。项目使用硬件随机数，追踪每一根蓍草的状态，支持一爻三变、六爻本卦与变卦生成、六十四卦映射，以及基于爻的二进制随机数生成。

> **“The number of the Great Expansion is fifty, of which forty-nine are used.  
> They are divided into two to symbolize the Two; one is set aside to symbolize the Three.  
> They are counted off in fours to symbolize the four seasons, and the remainders are placed between the fingers to symbolize the intercalary month.”**
>
> — *The Book of Changes, Great Treatise (Xici Zhuan)*

> **“大衍之数五十，其用四十有九。  
> 分而为二以象两，挂一以象三。  
> 揲之以四以象四时，归奇于扐以象闰。”**
>
> ——《周易·系辞上传》

---

## Language / 语言

- [简体中文](./README.zh-CN.md) | [English](./README.md)

---

## 项目简介

本项目使用 C++ 模拟传统大衍蓍草卜筮过程。

程序并非只使用一个整数表示“50根蓍草”，而是创建 **50个独立的 `YarrowStalk` 对象**。每根蓍草都有唯一编号和当前位置，因此可以追踪具体是哪一根蓍草进入左堆、右堆、挂一、归奇或被置于一旁。

这种对象级设计也为以后开发图形界面、动画和全过程回放提供了基础。

当前随机数实现直接使用 CPU 的 `RDSEED` 指令，不使用操作系统随机接口、软件伪随机数生成器、哈希算法或额外混合步骤。

---

## 主要功能

- 创建50个具有唯一编号的蓍草实例
- 随机选择“其一不用”的蓍草
- 完整模拟生成一爻所需的三变
- 每根参与蓍草独立决定进入左堆或右堆
- 从右堆随机选出一根执行“挂一”
- 左右两堆分别使用 Fisher–Yates 算法打乱
- 记录左右归奇所取出的具体蓍草编号
- 生成 `6`、`7`、`8`、`9` 四种爻值
- 连续生成六爻
- 计算本卦和变卦
- 根据上下卦映射文王六十四卦
- 输出一爻三变的完整调试记录
- 使用爻生成二进制随机数
- 提供中英文双语命令行帮助

---

## 随机数模型

所有随机操作统一通过 `HardwareRandom` 完成。

当前 Windows x64 实现使用：

```cpp
_rdseed64_step()
```

每一次随机请求都会尝试获取一个新的、成功返回的 `RDSEED` 结果，不会缓存或重复使用之前的输出。

硬件随机值用于：

- 选择最开始“其一不用”的蓍草；
- 让每根蓍草独立选择左堆或右堆；
- 从右堆随机选择“挂一”的具体蓍草；
- Fisher–Yates 洗牌中的每一次交换；
- 模拟中需要随机索引的其他操作。

### 当前实现说明

`nextInRange()` 当前使用取模方式转换范围。该方式简单直接，但在范围不是2的幂时，理论上存在极其微小的取模偏差。

本项目当前有意不加入哈希、拒绝采样、操作系统随机源或软件伪随机数生成器。

---

## 一爻算法

每次生成一爻时，都从50个独立蓍草对象开始。

### 初始化

1. 创建编号为 `0～49` 的50根蓍草。
2. 随机选择一根并标记为 `SetAside`，即“其一不用”。
3. 剩余49根参与三变。

### 每一变

每一变执行以下过程：

1. 每根当前参与的蓍草独立选择左堆或右堆。
2. 如果出现无法继续操作的极端分堆，则重新分堆。
3. 从右堆随机选择一根执行“挂一”。
4. 左堆执行 Fisher–Yates 洗牌。
5. 右堆执行 Fisher–Yates 洗牌。
6. 左右两堆分别以四根为一组进行揲四。
7. 普通余数为0时，按照4根处理。
8. 移出挂一、左余数和右余数。
9. 合并剩余蓍草并进入下一变。

三变完成后，剩余数量只能是：

| 剩余蓍草 | 爻值 | 类型 |
|---:|---:|---|
| 24 | 6 | 老阴，变爻 |
| 28 | 7 | 少阳，不变 |
| 32 | 8 | 少阴，不变 |
| 36 | 9 | 老阳，变爻 |

爻值计算方式为：

```text
爻值 = 最终剩余蓍草数 / 4
```

---

## 爻与二进制

| 爻值 | 类型 | 本爻位 | 变化后 |
|---:|---|---:|---:|
| 6 | 老阴 | 0 | 1 |
| 7 | 少阳 | 1 | 1 |
| 8 | 少阴 | 0 | 0 |
| 9 | 老阳 | 1 | 0 |

规则如下：

- 阴爻表示二进制 `0`；
- 阳爻表示二进制 `1`；
- `6` 和 `9` 是变爻；
- `7` 和 `8` 不发生变化。

---

## 六爻与六十四卦

完整一卦由六次一爻生成组成。

程序内部按照从下往上的顺序保存：

```text
lines[0] = 第一爻，最下方
lines[1] = 第二爻
lines[2] = 第三爻
lines[3] = 第四爻
lines[4] = 第五爻
lines[5] = 第六爻，最上方
```

第一至第三爻组成下卦，第四至第六爻组成上卦。

程序根据上下卦组合查询文王六十四卦映射，得到本卦。

老阴和老阳发生变化后，再生成变化后的六爻结构并查询变卦。

---

## 命令行用法

以下示例假设程序名为 `dayan.exe`。

在编译输出目录中：

```powershell
.\dayan.exe -h
```

在解决方案根目录中，典型的 Debug x64 路径为：

```powershell
.\x64\Debug\dayan.exe -h
```

### 查看帮助

```powershell
dayan -h
dayan --help
```

显示中英文双语帮助。

### 生成本卦和变卦

```powershell
dayan -bg
dayan --ben-gua
```

该命令会：

- 生成六爻；
- 按照从第六爻到第一爻的顺序显示；
- 标记变爻；
- 显示本卦的上卦和下卦；
- 查询本卦名称和卦序；
- 应用变爻；
- 查询变卦名称和卦序。

### 显示一爻三变详情

```powershell
dayan -detail
dayan --line-detail
```

该命令生成一爻，并输出完整三变记录，包括：

- “其一不用”的蓍草编号；
- 分二后的左右堆；
- 挂一的具体蓍草编号；
- 左右堆洗牌后的顺序；
- 左右余数编号；
- 每变移出数量；
- 每变结束数量；
- 剩余活动蓍草编号；
- 最终爻值。

该模式主要用于算法验证、学习、调试和未来动画开发。

### 使用爻生成随机数

```powershell
dayan -r 8
dayan --random 8
```

`-r` 后面的数字必须是不小于2的二次幂：

```text
2、4、8、16、32、64……
```

当 `N = 2ⁿ` 时，程序生成 `n` 个爻：

| 命令 | 生成爻数 | 二进制范围 | 最终结果范围 |
|---|---:|---:|---:|
| `dayan -r 2` | 1 | `0～1` | `1～2` |
| `dayan -r 4` | 2 | `0～3` | `1～4` |
| `dayan -r 8` | 3 | `0～7` | `1～8` |
| `dayan -r 16` | 4 | `0～15` | `1～16` |

第一次生成的爻对应最低有效位 `bit 0`。显示二进制时，按照通常格式将最高有效位放在最左侧。

输出示例：

```text
Original binary / 原二进制：   101
Changing mask / 变爻标记：      ^
Changed binary / 变化后二进制：100

Original binary decimal / 原二进制十进制值：5
Original result / 原随机结果（1～8）：6

Changed binary decimal / 变化后二进制十进制值：4
Changed result / 变化后结果（1～8）：5
```

原始二进制值范围是 `0～N-1`，最终输出的 `1～N` 结果通过以下方式得到：

```text
最终结果 = 二进制十进制值 + 1
```

变化后的二进制值来自变爻，用于显示变化结果。它不应被视为另一次相互独立的随机抽取。

---

## 项目结构

```text
DaYanEightTrigrams/
├── DaYanEightTrigrams.sln
├── DaYanEightTrigrams.vcxproj
├── DaYanEightTrigrams.vcxproj.filters
│
├── HardwareRandom.h
├── HardwareRandom.cpp
│
├── YarrowStalk.h
│
├── Divination.h
├── Divination.cpp
│
├── HexagramMapping.h
├── HexagramMapping.cpp
│
├── main.cpp
│
├── .gitignore
├── LICENSE.txt
└── README.md
```

---

## 文件职责

### `HardwareRandom.h / HardwareRandom.cpp`

提供统一的随机数接口。

当前实现使用 Windows MSVC 环境下的 x86-64 `RDSEED` 指令。

### `YarrowStalk.h`

定义单根蓍草对象，包括：

- 唯一编号；
- 当前所在位置；
- 卜筮过程中的状态变化。

### `Divination.h / Divination.cpp`

实现一次完整的一爻计算，包括：

- 创建50根蓍草；
- 随机选择“其一不用”；
- 三次变化；
- 分二；
- 挂一；
- 洗牌；
- 归奇；
- 数量验证；
- 保存详细过程记录。

### `HexagramMapping.h / HexagramMapping.cpp`

保存并处理：

- 八卦编码；
- 三爻阴阳到八卦的转换；
- 上下卦组合；
- 文王六十四卦映射。

### `main.cpp`

负责：

- 命令行参数解析；
- 中英文双语帮助；
- 六爻生成；
- 本卦和变卦显示；
- 一爻三变详情输出；
- 二进制随机数模式。

---

## Visual Studio 编译

### 环境要求

- Windows
- Visual Studio 2022
- MSVC C++ 编译器
- x64 编译目标
- C++14 或更高版本
- 支持 `RDSEED` 指令的 x86-64 CPU

### 编译步骤

1. 打开：

   ```text
   DaYanEightTrigrams.sln
   ```

2. 选择：

   ```text
   Configuration：Debug 或 Release
   Platform：x64
   ```

3. 执行：

   ```text
   Build → Build Solution
   ```

   快捷键：

   ```text
   Ctrl + Shift + B
   ```

4. 输出文件通常位于：

   ```text
   x64\Debug\
   ```

   或：

   ```text
   x64\Release\
   ```

如果 Visual Studio 中的 Target Name 已设置为 `dayan`，生成文件为：

```text
dayan.exe
```

否则请使用项目当前配置的可执行文件名。

---

## 平台限制

当前硬件随机数实现依赖 Windows、MSVC 和 x86-64 `RDSEED`。

`HardwareRandom` 的接口和实现已经分离，因此未来迁移到其他平台时，可以保留头文件接口，只替换平台相关的 `.cpp` 实现。

未来可以加入：

- Linux x86-64 RDSEED 实现；
- 其他CPU硬件随机数指令；
- 移动平台对应实现。

当前程序尚未在执行 `RDSEED` 前显式进行 CPUID 功能检测，因此只应在确认支持该指令的CPU上运行。

---

## 设计说明

### 独立蓍草对象

程序不是只用数字表示蓍草数量，而是为每根蓍草创建独立对象和稳定编号。

左右堆等容器只保存蓍草编号，不复制蓍草对象。

这样可以避免同一根蓍草同时出现在多个位置，也方便未来开发动画。

### 模拟人手分二

程序没有采用“在1到N-1之间均匀选择一个切点”的方法。

每根活动蓍草会独立选择左堆或右堆，因此左右数量通常集中在接近一半的位置，更符合人手将一束蓍草大致分成两堆的模拟目标。

这是本项目明确选择的模拟模型，其统计结果可能与其他抽象蓍草法实现存在差异。

### 洗牌记录

当前程序保存洗牌前和洗牌后的蓍草顺序。

目前尚未把 Fisher–Yates 中的每一次交换保存为独立动画事件。

---

## 后续计划

- 显示50根蓍草的图形界面
- 动画展示左右分堆
- 动画展示 Fisher–Yates 交换过程
- 保存完整操作事件历史
- 增加 CPU 指令支持检测
- 增加跨平台随机数实现
- 添加自动统计测试
- 添加六十四卦映射单元测试
- 提供 Release 编译版本和可下载程序

---

## 免责声明

本项目用于软件开发、算法模拟、文化研究和学习用途。

本项目不对预测、确定性或现实结果作出任何保证。

---

## 许可证

请参阅 [`LICENSE.txt`](./LICENSE.txt)。
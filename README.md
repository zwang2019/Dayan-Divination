# Dayan Yarrow-Stalk Divination
# 大衍蓍草卜筮模拟程序

A C++ simulation of the traditional Dayan yarrow-stalk divination process, with hardware-generated randomness, individual stalk tracking, hexagram mapping, changing-line calculation, and command-line random-number generation.

一个使用 C++ 编写的大衍蓍草卜筮模拟程序。项目使用硬件随机数，追踪每一根蓍草的状态，支持一爻三变、六爻本卦与变卦生成、六十四卦映射，以及基于爻的二进制随机数生成。

---

## Language / 语言

- [English](./README.md) | [简体中文](./README.zh-CN.md)

---

## Overview

This project simulates the yarrow-stalk method traditionally associated with the *I Ching*.

Instead of representing the stalks only as an integer count, the program creates **50 individual `YarrowStalk` objects**. Each stalk has a unique ID and a tracked location, allowing the program to record how specific stalks move during the divination process.

This design also provides a foundation for future graphical visualization and animation.

The current implementation generates randomness directly from the CPU `RDSEED` instruction. It does not use an operating-system random API, a software pseudorandom-number generator, hashing, or an additional mixing stage.

---

## Main Features

- Models 50 individual yarrow stalks with unique IDs
- Randomly selects the single stalk that is set aside
- Simulates all three transformations required to generate one line
- Assigns each active stalk independently to the left or right pile
- Randomly selects a stalk from the right pile for the “hang one” step
- Uses Fisher–Yates shuffling for both piles
- Tracks the exact stalk IDs removed as left and right remainders
- Generates line values `6`, `7`, `8`, and `9`
- Generates a complete six-line hexagram
- Calculates the original hexagram and changed hexagram
- Maps upper and lower trigrams to the 64 King Wen hexagrams
- Provides detailed transformation output for debugging and study
- Generates binary random values from yarrow lines
- Provides bilingual English/Chinese command-line help

---

## Randomness Model

All random decisions are routed through `HardwareRandom`.

The current Windows x64 implementation uses:

```cpp
_rdseed64_step()
```

Each request attempts to obtain a new successful `RDSEED` result. A previously returned value is not cached or reused for another decision.

Random hardware values are used for:

- selecting the initially set-aside stalk;
- assigning each active stalk to the left or right pile;
- selecting the specific stalk used for “hang one”;
- every Fisher–Yates shuffle step;
- other indexed selections required by the simulation.

### Important Implementation Note

The current `nextInRange()` implementation uses modulo reduction. This is simple and efficient, but non-power-of-two ranges can have an extremely small modulo bias.

The project intentionally does not add hashing, rejection sampling, an operating-system random source, or a software PRNG.

---

## Yarrow-Stalk Algorithm

Each generated line starts with 50 individual stalks.

### Initial Preparation

1. Create 50 stalks with IDs from `0` to `49`.
2. Randomly select one stalk and mark it as `SetAside`.
3. The remaining 49 stalks participate in the three transformations.

### One Transformation

For every transformation:

1. Each active stalk independently chooses the left or right pile.
2. Invalid extreme splits are rejected and repeated.
3. One random stalk is removed from the right pile for “hang one”.
4. The left pile is shuffled using Fisher–Yates.
5. The right pile is shuffled using Fisher–Yates.
6. Each pile is counted in groups of four.
7. A remainder of zero is treated as four stalks.
8. The hanging stalk and both remainders are removed.
9. The remaining stalks are merged and used in the next transformation.

After three transformations, the number of remaining stalks must be one of:

| Remaining stalks | Line value | Meaning |
|---:|---:|---|
| 24 | 6 | Old Yin, changing |
| 28 | 7 | Young Yang, unchanged |
| 32 | 8 | Young Yin, unchanged |
| 36 | 9 | Old Yang, changing |

The line value is calculated as:

```text
line value = remaining stalk count / 4
```

---

## Line Representation

| Value | Type | Original bit | Changed bit |
|---:|---|---:|---:|
| 6 | Old Yin | 0 | 1 |
| 7 | Young Yang | 1 | 1 |
| 8 | Young Yin | 0 | 0 |
| 9 | Old Yang | 1 | 0 |

- Yin is represented as binary `0`.
- Yang is represented as binary `1`.
- Values `6` and `9` are changing lines.
- Values `7` and `8` are unchanged lines.

---

## Hexagram Generation

A complete hexagram contains six generated lines.

The program stores them from bottom to top:

```text
lines[0] = first line, bottom
lines[1] = second line
lines[2] = third line
lines[3] = fourth line
lines[4] = fifth line
lines[5] = sixth line, top
```

The lower trigram is formed from lines 1–3, and the upper trigram is formed from lines 4–6.

The program then maps the upper/lower trigram pair to one of the 64 King Wen hexagrams.

Changing lines are transformed to create a second six-line pattern, which is mapped to the changed hexagram.

---

## Command-Line Usage

The examples below assume that the executable is named `dayan.exe`.

From the build-output directory:

```powershell
.\dayan.exe -h
```

From the solution directory in a typical Debug x64 build:

```powershell
.\x64\Debug\dayan.exe -h
```

### Display Help

```powershell
dayan -h
dayan --help
```

Displays bilingual English and Chinese help information.

### Generate an Original and Changed Hexagram

```powershell
dayan -bg
dayan --ben-gua
```

This command:

- generates six lines;
- displays the lines from top to bottom;
- marks changing lines;
- displays the upper and lower trigrams;
- identifies the original hexagram;
- applies changing lines;
- identifies the changed hexagram.

### Display One Complete Line Calculation

```powershell
dayan -detail
dayan --line-detail
```

This command generates one line and prints detailed information for all three transformations, including:

- the set-aside stalk ID;
- left and right piles after splitting;
- the selected hanging stalk;
- both shuffled pile orders;
- left and right remainder IDs;
- removed counts;
- remaining active stalk IDs;
- final line value.

This mode is mainly intended for verification, study, debugging, and future animation development.

### Generate a Binary Random Number

```powershell
dayan -r 8
dayan --random 8
```

The number after `-r` must be a power of two and at least 2:

```text
2, 4, 8, 16, 32, 64, ...
```

For a range of `N = 2ⁿ`, the program generates `n` lines:

| Command | Lines generated | Binary values | User-facing range |
|---|---:|---:|---:|
| `dayan -r 2` | 1 | `0–1` | `1–2` |
| `dayan -r 4` | 2 | `0–3` | `1–4` |
| `dayan -r 8` | 3 | `0–7` | `1–8` |
| `dayan -r 16` | 4 | `0–15` | `1–16` |

The first generated line corresponds to `bit 0`, the least significant bit. Binary output is displayed conventionally with the most significant bit on the left.

Example:

```text
Original binary / 原二进制：   101
Changing mask / 变爻标记：      ^
Changed binary / 变化后二进制：100

Original binary decimal / 原二进制十进制值：5
Original result / 原随机结果（1–8）：6

Changed binary decimal / 变化后二进制十进制值：4
Changed result / 变化后结果（1–8）：5
```

The raw binary value is in the range `0` to `N - 1`. The user-facing result is calculated as:

```text
result = binary decimal value + 1
```

The changed binary value is derived from changing lines and is displayed as a secondary transformation result. It should not be interpreted as a separate independent random draw.

---

## Project Structure

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

## File Responsibilities

### `HardwareRandom.h / HardwareRandom.cpp`

Provides the platform-specific random-number interface.

The current implementation uses the x86-64 `RDSEED` instruction through MSVC intrinsics.

### `YarrowStalk.h`

Defines one individual yarrow stalk, including:

- unique ID;
- current location;
- state transitions used by the simulation.

### `Divination.h / Divination.cpp`

Implements one complete line calculation:

- 50-stalk initialization;
- random set-aside selection;
- three transformations;
- pile splitting;
- hanging one;
- shuffling;
- remainder removal;
- validation;
- detailed operation records.

### `HexagramMapping.h / HexagramMapping.cpp`

Defines:

- the eight trigrams;
- binary trigram encoding;
- upper/lower trigram combinations;
- the 64 King Wen hexagram mappings.

### `main.cpp`

Implements:

- command-line parsing;
- bilingual help;
- six-line generation;
- original and changed hexagram output;
- detailed line output;
- binary random-number mode.

---

## Building with Visual Studio

### Requirements

- Windows
- Visual Studio 2022
- MSVC C++ compiler
- x64 build target
- C++14 or later
- An x86-64 CPU that supports the `RDSEED` instruction

### Build Steps

1. Open:

   ```text
   DaYanEightTrigrams.sln
   ```

2. Select:

   ```text
   Configuration: Debug or Release
   Platform: x64
   ```

3. Build the solution:

   ```text
   Build → Build Solution
   ```

   Shortcut:

   ```text
   Ctrl + Shift + B
   ```

4. The executable will normally be created under:

   ```text
   x64\Debug\
   ```

   or:

   ```text
   x64\Release\
   ```

If the Visual Studio Target Name is set to `dayan`, the output file will be:

```text
dayan.exe
```

Otherwise, use the executable name configured by the project.

---

## Platform Limitations

The current hardware-random implementation is Windows/MSVC/x86-64 specific.

The public `HardwareRandom` interface is separated from its implementation so that future ports can replace only the platform-specific source file.

Possible future backends include:

- Linux x86-64 with RDSEED intrinsics;
- other CPU hardware-random instructions;
- platform-native implementations for mobile systems.

The current program does not yet perform an explicit CPUID feature check before executing `RDSEED`. It should therefore only be run on a CPU known to support the instruction.

---

## Design Notes

### Individual Stalk Objects

The program does not represent the yarrow stalks only as a count.

Every stalk is an individual object with a stable ID. Pile containers store stalk IDs instead of copying stalk objects.

This prevents one physical stalk from accidentally appearing in multiple locations and makes the model suitable for future visualization.

### Human-Like Pile Splitting

The split is not produced by selecting one uniformly random cut position.

Instead, every active stalk independently chooses the left or right pile. This causes pile sizes to cluster around an approximately balanced split and is intended to resemble a human dividing a bundle into two roughly similar piles.

This is an explicit simulation choice. Statistical results may differ from other abstract implementations of the yarrow-stalk method.

### Shuffle Records

The current transformation results preserve the pile order before and after shuffling.

The program does not yet preserve every individual Fisher–Yates swap as a separate animation event.

---

## Planned Improvements

- Graphical visualization of all 50 stalks
- Animated left/right pile movement
- Animated Fisher–Yates swaps
- Complete event-history recording
- Explicit CPU feature detection
- Cross-platform random backends
- Automated statistical tests
- Unit tests for all 64 hexagram mappings
- Release builds and downloadable binaries

---

## Disclaimer

This project is intended for software development, cultural study, algorithm simulation, and educational use.

It does not make claims about prediction, certainty, or real-world outcomes.

---

## License

See [`LICENSE.txt`](./LICENSE.txt).
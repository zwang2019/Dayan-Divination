# Dayan Divination

A C++ simulation of the traditional Da Yan yarrow-stalk divination method.

## Features

- Models 50 individual yarrow stalks
- Randomly selects one stalk to set aside
- Assigns each active stalk independently to the left or right pile
- Uses Intel RDSEED hardware-generated randomness
- Uses Fisher–Yates shuffling
- Simulates the three transformations required to generate one line
- Tracks individual stalk IDs for future visualization and animation

## Requirements

- Windows
- Visual Studio 2022
- x64 build target
- A CPU supporting the RDSEED instruction

## Current output

The program generates one line with a value of:

- 6: Old Yin
- 7: Young Yang
- 8: Young Yin
- 9: Old Yang
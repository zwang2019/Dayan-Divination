#pragma once

#include <cstdint>

/*
 * HardwareRandom
 *
 * 对外提供硬件随机数接口。
 *
 * 当前平台实现位于 HardwareRandom.cpp，
 * 使用 Intel CPU 的 RDSEED 指令。
 *
 * 以后迁移平台时，可以保留本头文件不变，
 * 只替换 HardwareRandom.cpp 的具体实现。
 */
class HardwareRandom
{
public:
    /*
     * 获取一个新的 64 位原始硬件随机数。
     *
     * 每次调用都会重新执行 RDSEED，
     * 不缓存、不复用之前的结果。
     */
    static std::uint64_t nextRaw();

    /*
     * 获取 0～maxInclusive 之间的随机整数，
     * 包含 0 和 maxInclusive。
     *
     * 例如：
     *     nextInRange(49)  返回 0～49
     *     nextInRange(5)   返回 0～5
     */
    static std::uint64_t nextInRange(
        std::uint64_t maxInclusive
    );

    /*
     * 使用一个新的硬件随机数判断奇偶。
     *
     * 返回：
     *     true  表示随机数为奇数
     *     false 表示随机数为偶数
     */
    static bool nextIsOdd();
};
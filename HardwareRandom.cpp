#include "HardwareRandom.h"

#include <immintrin.h>
#include <limits>
#include <stdexcept>


std::uint64_t HardwareRandom::nextRaw()
{
    /*
     * RDSEED 可能暂时没有准备好新的输出，
     * 因此最多尝试 128 次。
     *
     * 每一次循环都会重新执行一次 RDSEED。
     */
    for (int attempt = 0; attempt < 128; ++attempt) {
        unsigned __int64 generatedValue;

        /*
         * 返回非零值表示本次 RDSEED 成功。
         */
        if (_rdseed64_step(&generatedValue)) {
            return static_cast<std::uint64_t>(
                generatedValue
                );
        }
    }

    /*
     * 128 次全部失败时抛出异常。
     */
    throw std::runtime_error(
        "RDSEED failed after 128 attempts"
    );
}


std::uint64_t HardwareRandom::nextInRange(
    std::uint64_t maxInclusive
)
{
    /*
     * 如果 maxInclusive 已经是 uint64_t 最大值，
     * 就不能再执行 maxInclusive + 1，
     * 否则会发生无符号整数溢出。
     *
     * 此时直接返回完整的 64 位随机值。
     */
    if (
        maxInclusive ==
        std::numeric_limits<std::uint64_t>::max()
        ) {
        return nextRaw();
    }

    /*
     * 例如 maxInclusive = 49：
     *
     * nextRaw() % 50
     *
     * 返回范围为 0～49。
     */
    return nextRaw() % (maxInclusive + 1);
}


bool HardwareRandom::nextIsOdd()
{
    /*
     * 每次调用都会重新执行 RDSEED。
     *
     * 偶数 % 2 = 0
     * 奇数 % 2 = 1
     */
    return nextRaw() % 2 != 0;
}
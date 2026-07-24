#pragma once

#include <array>
#include <cstdint>


/*
 * 三个二进制位按照“自下而上”的顺序表示八卦：
 *
 * bit 0：下爻
 * bit 1：中爻
 * bit 2：上爻
 *
 * 0表示阴，1表示阳。
 */
enum class Trigram : std::uint8_t
{
    Kun = 0b000,  // 坤 ☷
    Zhen = 0b001,  // 震 ☳
    Kan = 0b010,  // 坎 ☵
    Dui = 0b011,  // 兑 ☱
    Gen = 0b100,  // 艮 ☶
    Li = 0b101,  // 离 ☲
    Xun = 0b110,  // 巽 ☴
    Qian = 0b111   // 乾 ☰
};


struct TrigramInfo
{
    Trigram trigram;
    const char* name;
    const char* symbol;
};


struct HexagramInfo
{
    /*
     * 文王卦序编号：1～64。
     */
    int number;

    /*
     * 卦名，例如：
     *
     * 豫
     * 谦
     */
    const char* name;

    /*
     * 下三爻组成的下卦。
     */
    Trigram lower;

    /*
     * 上三爻组成的上卦。
     */
    Trigram upper;
};


class HexagramMapping
{
public:
    /*
     * 根据自下而上的三爻生成八卦。
     */
    static Trigram trigramFromLines(
        bool bottomYang,
        bool middleYang,
        bool topYang
    );

    /*
     * 根据六爻阴阳状态查找六十四卦。
     *
     * lines[0]：第一爻
     * lines[5]：第六爻
     */
    static const HexagramInfo& fromLines(
        const std::array<bool, 6>& lines
    );

    /*
     * 根据上下卦查找六十四卦。
     */
    static const HexagramInfo& find(
        Trigram lower,
        Trigram upper
    );

    /*
     * 获取八卦名称和符号。
     */
    static const TrigramInfo& trigramInfo(
        Trigram trigram
    );
};
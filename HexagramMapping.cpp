#include "HexagramMapping.h"

#include <array>
#include <cstddef>
#include <stdexcept>


namespace
{
    /*
     * 下标必须与 Trigram 的0～7编码一致。
     */
    const std::array<TrigramInfo, 8> kTrigrams = { {
        { Trigram::Kun,  "坤", "☷" },
        { Trigram::Zhen, "震", "☳" },
        { Trigram::Kan,  "坎", "☵" },
        { Trigram::Dui,  "兑", "☱" },
        { Trigram::Gen,  "艮", "☶" },
        { Trigram::Li,   "离", "☲" },
        { Trigram::Xun,  "巽", "☴" },
        { Trigram::Qian, "乾", "☰" }
    } };


    /*
     * 六十四卦映射。
     *
     * 每一项依次保存：
     *
     * 文王卦序
     * 卦名
     * 下卦
     * 上卦
     */
    const std::array<HexagramInfo, 64> kHexagrams = { {
        {  1, "乾",   Trigram::Qian, Trigram::Qian },
        {  2, "坤",   Trigram::Kun,   Trigram::Kun   },
        {  3, "屯",   Trigram::Zhen,  Trigram::Kan   },
        {  4, "蒙",   Trigram::Kan,   Trigram::Gen   },
        {  5, "需",   Trigram::Qian,  Trigram::Kan   },
        {  6, "讼",   Trigram::Kan,   Trigram::Qian },
        {  7, "师",   Trigram::Kan,   Trigram::Kun   },
        {  8, "比",   Trigram::Kun,   Trigram::Kan   },
        {  9, "小畜", Trigram::Qian,  Trigram::Xun   },
        { 10, "履",   Trigram::Dui,   Trigram::Qian },
        { 11, "泰",   Trigram::Qian,  Trigram::Kun   },
        { 12, "否",   Trigram::Kun,   Trigram::Qian },
        { 13, "同人", Trigram::Li,    Trigram::Qian },
        { 14, "大有", Trigram::Qian,  Trigram::Li    },
        { 15, "谦",   Trigram::Gen,   Trigram::Kun   },
        { 16, "豫",   Trigram::Kun,   Trigram::Zhen  },
        { 17, "随",   Trigram::Zhen,  Trigram::Dui   },
        { 18, "蛊",   Trigram::Xun,   Trigram::Gen   },
        { 19, "临",   Trigram::Dui,   Trigram::Kun   },
        { 20, "观",   Trigram::Kun,   Trigram::Xun   },
        { 21, "噬嗑", Trigram::Zhen,  Trigram::Li    },
        { 22, "贲",   Trigram::Li,    Trigram::Gen   },
        { 23, "剥",   Trigram::Kun,   Trigram::Gen   },
        { 24, "复",   Trigram::Zhen,  Trigram::Kun   },
        { 25, "无妄", Trigram::Zhen,  Trigram::Qian },
        { 26, "大畜", Trigram::Qian,  Trigram::Gen   },
        { 27, "颐",   Trigram::Zhen,  Trigram::Gen   },
        { 28, "大过", Trigram::Xun,   Trigram::Dui   },
        { 29, "坎",   Trigram::Kan,   Trigram::Kan   },
        { 30, "离",   Trigram::Li,    Trigram::Li    },
        { 31, "咸",   Trigram::Gen,   Trigram::Dui   },
        { 32, "恒",   Trigram::Xun,   Trigram::Zhen  },
        { 33, "遁",   Trigram::Gen,   Trigram::Qian },
        { 34, "大壮", Trigram::Qian,  Trigram::Zhen  },
        { 35, "晋",   Trigram::Kun,   Trigram::Li    },
        { 36, "明夷", Trigram::Li,    Trigram::Kun   },
        { 37, "家人", Trigram::Li,    Trigram::Xun   },
        { 38, "睽",   Trigram::Dui,   Trigram::Li    },
        { 39, "蹇",   Trigram::Gen,   Trigram::Kan   },
        { 40, "解",   Trigram::Kan,   Trigram::Zhen  },
        { 41, "损",   Trigram::Dui,   Trigram::Gen   },
        { 42, "益",   Trigram::Zhen,  Trigram::Xun   },
        { 43, "夬",   Trigram::Qian,  Trigram::Dui   },
        { 44, "姤",   Trigram::Xun,   Trigram::Qian },
        { 45, "萃",   Trigram::Kun,   Trigram::Dui   },
        { 46, "升",   Trigram::Xun,   Trigram::Kun   },
        { 47, "困",   Trigram::Kan,   Trigram::Dui   },
        { 48, "井",   Trigram::Xun,   Trigram::Kan   },
        { 49, "革",   Trigram::Li,    Trigram::Dui   },
        { 50, "鼎",   Trigram::Xun,   Trigram::Li    },
        { 51, "震",   Trigram::Zhen,  Trigram::Zhen  },
        { 52, "艮",   Trigram::Gen,   Trigram::Gen   },
        { 53, "渐",   Trigram::Gen,   Trigram::Xun   },
        { 54, "归妹", Trigram::Dui,   Trigram::Zhen  },
        { 55, "丰",   Trigram::Li,    Trigram::Zhen  },
        { 56, "旅",   Trigram::Gen,   Trigram::Li    },
        { 57, "巽",   Trigram::Xun,   Trigram::Xun   },
        { 58, "兑",   Trigram::Dui,   Trigram::Dui   },
        { 59, "涣",   Trigram::Kan,   Trigram::Xun   },
        { 60, "节",   Trigram::Dui,   Trigram::Kan   },
        { 61, "中孚", Trigram::Dui,   Trigram::Xun   },
        { 62, "小过", Trigram::Gen,   Trigram::Zhen  },
        { 63, "既济", Trigram::Li,    Trigram::Kan   },
        { 64, "未济", Trigram::Kan,   Trigram::Li    }
    } };
}


Trigram HexagramMapping::trigramFromLines(
    bool bottomYang,
    bool middleYang,
    bool topYang
)
{
    std::uint8_t code = 0;

    if (bottomYang) {
        code |= 0b001;
    }

    if (middleYang) {
        code |= 0b010;
    }

    if (topYang) {
        code |= 0b100;
    }

    return static_cast<Trigram>(code);
}


const HexagramInfo& HexagramMapping::fromLines(
    const std::array<bool, 6>& lines
)
{
    Trigram lower = trigramFromLines(
        lines[0],
        lines[1],
        lines[2]
    );

    Trigram upper = trigramFromLines(
        lines[3],
        lines[4],
        lines[5]
    );

    return find(lower, upper);
}


const HexagramInfo& HexagramMapping::find(
    Trigram lower,
    Trigram upper
)
{
    for (const HexagramInfo& hexagram : kHexagrams) {
        if (
            hexagram.lower == lower &&
            hexagram.upper == upper
            ) {
            return hexagram;
        }
    }

    throw std::logic_error(
        "Hexagram mapping was not found"
    );
}


const TrigramInfo& HexagramMapping::trigramInfo(
    Trigram trigram
)
{
    std::size_t index =
        static_cast<std::size_t>(trigram);

    if (index >= kTrigrams.size()) {
        throw std::out_of_range(
            "Invalid trigram value"
        );
    }

    return kTrigrams[index];
}
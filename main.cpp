#include "Divination.h"
#include "HexagramMapping.h"

#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif


namespace
{
    /*
     * Windows控制台使用UTF-8输出中文。
     */
    void configureConsoleEncoding()
    {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif
    }


    void printIds(
        const std::vector<int>& ids
    )
    {
        std::cout << '[';

        for (
            std::size_t index = 0;
            index < ids.size();
            ++index
            ) {
            std::cout << ids[index];

            if (index + 1 < ids.size()) {
                std::cout << ", ";
            }
        }

        std::cout << ']';
    }


    const char* lineName(int value)
    {
        switch (value) {
        case 6:
            return "Old Yin";

        case 7:
            return "Young Yang";

        case 8:
            return "Young Yin";

        case 9:
            return "Old Yang";

        default:
            return "Invalid";
        }
    }


    /*
     * 本卦中的阴阳性质。
     *
     * 7、9为阳；
     * 6、8为阴。
     */
    bool isOriginalYang(int value)
    {
        return value == 7 || value == 9;
    }


    /*
     * 判断是否为变爻。
     */
    bool isChangingLine(int value)
    {
        return value == 6 || value == 9;
    }


    /*
     * 计算变化后的阴阳性质。
     *
     * 6：老阴，阴变阳
     * 9：老阳，阳变阴
     */
    bool isChangedYang(int value)
    {
        switch (value) {
        case 6:
            return true;

        case 7:
            return true;

        case 8:
            return false;

        case 9:
            return false;

        default:
            throw std::logic_error(
                "Invalid line value"
            );
        }
    }


    const char* lineFigure(bool yang)
    {
        if (yang) {
            return "---------";
        }

        return "---- ----";
    }

     /*
     * 检查一个整数是否为2的正整数次方。
     *
     * 本程序要求至少为2：
     *
     * 2、4、8、16、32……
     *
     * 1虽然数学上等于2的0次方，
     * 但随机范围1～1没有实际意义，因此不接受。
     */
    bool isPowerOfTwo(
        std::uint64_t value
    ) noexcept
    {
        return (
            value >= 2 &&
            (value & (value - 1)) == 0
            );
    }


    /*
     * 计算2的幂对应多少个二进制位。
     *
     * 例如：
     *
     * 2  = 2^1 → 1位
     * 8  = 2^3 → 3位
     * 16 = 2^4 → 4位
     */
    std::size_t calculateBitCount(
        std::uint64_t range
    )
    {
        std::size_t bitCount = 0;

        while (range > 1) {
            range >>= 1;
            ++bitCount;
        }

        return bitCount;
    }


    /*
     * 解析-r命令后面的数字。
     *
     * 要求：
     *
     * 1. 必须是纯正整数
     * 2. 必须至少为2
     * 3. 必须是2的幂
     */
    std::uint64_t parsePowerOfTwoRange(
        const char* text
    )
    {
        std::string input = text;

        if (
            input.empty() ||
            input.front() == '-' ||
            input.front() == '+'
            ) {
            throw std::invalid_argument(
                "The range must be a positive integer. "
                "/ 范围必须是正整数。"
            );
        }

        std::size_t processedCharacters = 0;
        unsigned long long parsedValue = 0;

        try {
            parsedValue = std::stoull(
                input,
                &processedCharacters,
                10
            );
        }
        catch (const std::exception&) {
            throw std::invalid_argument(
                "The range is not a valid 64-bit integer. "
                "/ 范围不是有效的64位整数。"
            );
        }

        /*
         * 防止输入类似：
         *
         * 16abc
         */
        if (processedCharacters != input.size()) {
            throw std::invalid_argument(
                "The range must contain digits only. "
                "/ 范围参数只能包含数字。"
            );
        }

        std::uint64_t range =
            static_cast<std::uint64_t>(
                parsedValue
                );

        if (!isPowerOfTwo(range)) {
            throw std::invalid_argument(
                "The range must be a power of two and at least 2, "
                "such as 2, 4, 8, 16, 32 or 64. "
                "/ 范围必须是不小于2的二次幂，例如2、4、8、16、32或64。"
            );
        }

        return range;
    }


    /*
     * 六次调用Divination，每次生成一爻。
     *
     * lines[0]：第一爻，最下面
     * lines[5]：第六爻，最上面
     */
    std::array<LineResult, 6> generateSixLines()
    {
        std::array<LineResult, 6> lines;

        for (
            std::size_t index = 0;
            index < lines.size();
            ++index
            ) {
            lines[index] =
                Divination::generateLine();
        }

        return lines;
    }


    std::array<bool, 6> buildOriginalLines(
        const std::array<LineResult, 6>& lines
    )
    {
        std::array<bool, 6> result = {};

        for (
            std::size_t index = 0;
            index < lines.size();
            ++index
            ) {
            result[index] =
                isOriginalYang(
                    lines[index].lineValue
                );
        }

        return result;
    }


    std::array<bool, 6> buildChangedLines(
        const std::array<LineResult, 6>& lines
    )
    {
        std::array<bool, 6> result = {};

        for (
            std::size_t index = 0;
            index < lines.size();
            ++index
            ) {
            result[index] =
                isChangedYang(
                    lines[index].lineValue
                );
        }

        return result;
    }


    /*
     * -bg命令：
     *
     * 生成本卦和变卦。
     */
    void runBenGuaCommand()
    {
        std::array<LineResult, 6> lines =
            generateSixLines();

        std::array<bool, 6> originalLines =
            buildOriginalLines(lines);

        std::array<bool, 6> changedLines =
            buildChangedLines(lines);

        const HexagramInfo& originalHexagram =
            HexagramMapping::fromLines(
                originalLines
            );

        const HexagramInfo& changedHexagram =
            HexagramMapping::fromLines(
                changedLines
            );

        const TrigramInfo& originalUpper =
            HexagramMapping::trigramInfo(
                originalHexagram.upper
            );

        const TrigramInfo& originalLower =
            HexagramMapping::trigramInfo(
                originalHexagram.lower
            );

        const TrigramInfo& changedUpper =
            HexagramMapping::trigramInfo(
                changedHexagram.upper
            );

        const TrigramInfo& changedLower =
            HexagramMapping::trigramInfo(
                changedHexagram.lower
            );


        const char* chineseLineNames[6] = {
            "第一爻",
            "第二爻",
            "第三爻",
            "第四爻",
            "第五爻",
            "第六爻"
        };

        const char* englishLineNames[6] = {
            "1st line",
            "2nd line",
            "3rd line",
            "4th line",
            "5th line",
            "6th line"
        };


        std::cout
            << "本卦                         变卦\n"
            << "--------------------------------------------------------\n";


        /*
         * 六爻在内存中自下而上保存，
         * 但显示时从第六爻打印到第一爻，
         * 才符合通常看到的卦象方向。
         */
        for (int index = 5; index >= 0; --index) {
            int value =
                lines[
                    static_cast<std::size_t>(index)
                ].lineValue;

            bool originalYang =
                originalLines[
                    static_cast<std::size_t>(index)
                ];

            bool changedYang =
                changedLines[
                    static_cast<std::size_t>(index)
                ];

            std::cout
                << chineseLineNames[index]
                << " ("
                << englishLineNames[index]
                << ")："
                << value
                << "    "
                << lineFigure(originalYang);

            if (isChangingLine(value)) {
                std::cout << "   ==变==>   ";
            }
            else {
                std::cout << "    --->     ";
            }

            std::cout
                << lineFigure(changedYang);


            /*
             * 第五爻所在行显示上卦。
             */
            if (index == 4) {
                std::cout
                    << "    上卦："
                    << originalUpper.name
                    << originalUpper.symbol
                    << " -> "
                    << changedUpper.name
                    << changedUpper.symbol;
            }


            /*
             * 第二爻所在行显示下卦。
             */
            if (index == 1) {
                std::cout
                    << "    下卦："
                    << originalLower.name
                    << originalLower.symbol
                    << " -> "
                    << changedLower.name
                    << changedLower.symbol;
            }

            std::cout << '\n';
        }


        std::cout
            << "--------------------------------------------------------\n";

        std::cout
            << "本卦：第"
            << originalHexagram.number
            << "卦 "
            << originalHexagram.name
            << "（上"
            << originalUpper.name
            << "下"
            << originalLower.name
            << "）\n";

        std::cout
            << "变卦：第"
            << changedHexagram.number
            << "卦 "
            << changedHexagram.name
            << "（上"
            << changedUpper.name
            << "下"
            << changedLower.name
            << "）\n";


        bool hasChangingLine = false;

        for (const LineResult& line : lines) {
            if (isChangingLine(line.lineValue)) {
                hasChangingLine = true;
                break;
            }
        }

        if (!hasChangingLine) {
            std::cout
                << "本次没有变爻，变卦与本卦相同。\n";
        }
    }


    /*
     * -detail命令：
     *
     * 保留原来的一爻三变详细信息。
     */
    void runDetailCommand()
    {
        LineResult line =
            Divination::generateLine();

        std::cout
            << "Set-aside stalk ID: "
            << line.setAsideId
            << "\n\n";

        for (
            const TransformationResult& transformation
            : line.transformations
            ) {
            std::cout
                << "Transformation "
                << transformation.transformationNumber
                << '\n';

            std::cout
                << "Starting count: "
                << transformation.startingCount
                << '\n';

            std::cout
                << "Left pile after split:  ";

            printIds(
                transformation.leftAfterSplit
            );

            std::cout
                << "\nRight pile after split: ";

            printIds(
                transformation.rightAfterSplit
            );

            std::cout
                << "\nHung stalk ID: "
                << transformation.hungId
                << '\n';

            std::cout
                << "Left after shuffle:     ";

            printIds(
                transformation.leftAfterShuffle
            );

            std::cout
                << "\nRight after shuffle:    ";

            printIds(
                transformation.rightAfterShuffle
            );

            std::cout
                << "\nLeft remainder IDs:     ";

            printIds(
                transformation.leftRemainderIds
            );

            std::cout
                << "\nRight remainder IDs:    ";

            printIds(
                transformation.rightRemainderIds
            );

            std::cout
                << "\nRemoved count: "
                << transformation.removedCount
                << '\n';

            std::cout
                << "Ending count: "
                << transformation.endingCount
                << '\n';

            std::cout
                << "Remaining active IDs:   ";

            printIds(
                transformation.remainingActiveIds
            );

            std::cout << "\n\n";
        }

        std::cout
            << "Final line value: "
            << line.lineValue
            << '\n';

        std::cout
            << "Line type: "
            << lineName(line.lineValue)
            << '\n';
    }

    /*
     * -r命令：
     *
     * 使用蓍草生成二进制随机数。
     *
     * 第一次生成的爻对应bit 0，即最低有效位。
     * 最后生成的爻对应最高有效位。
     */
    void runRandomCommand(
        const char* rangeText
    )
    {
        std::uint64_t range =
            parsePowerOfTwoRange(
                rangeText
            );

        std::size_t bitCount =
            calculateBitCount(
                range
            );


        /*
         * 为每一个二进制位生成一爻。
         *
         * lines[0]对应bit 0。
         * lines[1]对应bit 1。
         */
        std::vector<LineResult> lines;

        lines.reserve(bitCount);

        for (
            std::size_t index = 0;
            index < bitCount;
            ++index
            ) {
            lines.push_back(
                Divination::generateLine()
            );
        }


        std::uint64_t originalDecimal = 0;
        std::uint64_t changedDecimal = 0;

        std::string originalBinary;
        std::string changedBinary;
        std::string changingMask;

        originalBinary.reserve(bitCount);
        changedBinary.reserve(bitCount);
        changingMask.reserve(bitCount);


        /*
         * 二进制显示时通常把最高位放在最左边，
         * 所以这里从最后一个爻向第一个爻输出。
         */
        for (
            std::size_t position = bitCount;
            position-- > 0;
            ) {
            int lineValue =
                lines[position].lineValue;

            bool originalBit =
                isOriginalYang(
                    lineValue
                );

            bool changedBit =
                isChangedYang(
                    lineValue
                );

            bool changing =
                isChangingLine(
                    lineValue
                );


            originalBinary.push_back(
                originalBit ? '1' : '0'
            );

            changedBinary.push_back(
                changedBit ? '1' : '0'
            );

            /*
             * ^表示这一位对应的是变爻。
             */
            changingMask.push_back(
                changing ? '^' : ' '
            );


            if (originalBit) {
                originalDecimal |= (
                    std::uint64_t{ 1 }
                    << position
                    );
            }

            if (changedBit) {
                changedDecimal |= (
                    std::uint64_t{ 1 }
                    << position
                    );
            }
        }


        /*
         * 二进制值本身范围为0～N-1。
         *
         * 加1后映射为用户要求的1～N。
         */
        std::uint64_t originalResult =
            originalDecimal + 1;

        std::uint64_t changedResult =
            changedDecimal + 1;


        std::cout
            << "Yarrow binary random number\n"
            << "蓍草二进制随机数\n\n";

        std::cout
            << "Requested range / 请求范围：1～"
            << range
            << '\n';

        std::cout
            << "Required lines / 所需爻数："
            << bitCount
            << '\n';

        std::cout
            << "Bit order / 位序：最高位在左，第一爻对应bit 0。\n"
            << "               The first generated line is bit 0.\n\n";


        std::cout
            << "Original binary / 原二进制：   "
            << originalBinary
            << '\n';

        std::cout
            << "Changing mask / 变爻标记：     "
            << changingMask
            << '\n';

        std::cout
            << "Changed binary / 变化后二进制："
            << changedBinary
            << "\n\n";


        std::cout
            << "Original binary decimal / 原二进制十进制值："
            << originalDecimal
            << '\n';

        std::cout
            << "Original result / 原随机结果（1～"
            << range
            << "）："
            << originalResult
            << "\n\n";


        std::cout
            << "Changed binary decimal / 变化后二进制十进制值："
            << changedDecimal
            << '\n';

        std::cout
            << "Changed result / 变化后结果（1～"
            << range
            << "）："
            << changedResult
            << "\n\n";


        /*
         * 逐位显示每一个爻和对应的二进制变化。
         */
        std::cout
            << "Bit details / 各位详情：\n";

        for (
            std::size_t position = bitCount;
            position-- > 0;
            ) {
            int lineValue =
                lines[position].lineValue;

            bool originalBit =
                isOriginalYang(
                    lineValue
                );

            bool changedBit =
                isChangedYang(
                    lineValue
                );

            bool changing =
                isChangingLine(
                    lineValue
                );

            std::cout
                << (changing ? "* " : "  ")
                << "bit "
                << position
                << " / 第"
                << position + 1
                << "次生成的爻："
                << lineValue
                << "    "
                << (originalBit ? '1' : '0');

            if (changing) {
                std::cout
                    << " -> "
                    << (changedBit ? '1' : '0')
                    << "    changing / 变爻";
            }
            else {
                std::cout
                    << " -> "
                    << (changedBit ? '1' : '0')
                    << "    unchanged / 不变";
            }

            std::cout << '\n';
        }
    }


    void printUsage(
        const char* executableName
    )
    {
        std::cout
            << "Dayan Yarrow-Stalk Divination\n"
            << "大衍蓍草卜筮程序\n\n"

            << "Usage / 用法:\n\n"

            << "  "
            << executableName
            << " -bg\n"
            << "  "
            << executableName
            << " --ba-gua\n"
            << "      Generate six lines and display the original and changed hexagrams.\n"
            << "      生成六爻，并显示本卦、变爻和变卦。\n\n"

            << "  "
            << executableName
            << " -detail\n"
            << "  "
            << executableName
            << " --line-detail\n"
            << "      Generate one line and display all three yarrow transformations.\n"
            << "      生成一爻，并显示三变过程中每根蓍草的详细记录。\n\n"

            << "  "
            << executableName
            << " -r <N>\n"
            << "  "
            << executableName
            << " --random <N>\n"
            << "      Generate a random number from 1 to N using yarrow lines as binary bits.\n"
            << "      使用蓍草爻作为二进制位，生成1到N之间的随机数。\n"
            << "      N must be a power of two and at least 2.\n"
            << "      N必须是不小于2的二次幂，例如2、4、8、16、32或64。\n\n"

            << "  "
            << executableName
            << " -h\n"
            << "  "
            << executableName
            << " --help\n"
            << "      Display this help information.\n"
            << "      显示本帮助信息。\n\n"

            << "Line values / 爻值说明:\n"
            << "      6 - Old Yin, changing from 0 to 1\n"
            << "          老阴，二进制从0变为1\n"
            << "      7 - Young Yang, remains 1\n"
            << "          少阳，保持为1\n"
            << "      8 - Young Yin, remains 0\n"
            << "          少阴，保持为0\n"
            << "      9 - Old Yang, changing from 1 to 0\n"
            << "          老阳，二进制从1变为0\n\n"

            << "Examples / 示例:\n"
            << "      "
            << executableName
            << " -bg\n"
            << "      "
            << executableName
            << " -detail\n"
            << "      "
            << executableName
            << " -r 8\n"
            << "      "
            << executableName
            << " -r 16\n";
    }
}


int main(
    int argc,
    char* argv[]
)
{
    configureConsoleEncoding();

    try {
        if (argc < 2) {
            printUsage("dayan");
            return 0;
        }

        std::string command =
            argv[1];

        if (
            command == "-bg" ||
            command == "--ba-gua"
            ) {
            runBenGuaCommand();
            return 0;
        }

        if (
            command == "-detail" ||
            command == "--line-detail"
            ) {
            runDetailCommand();
            return 0;
        }

        if (
            command == "-r" ||
            command == "--random"
            ) {
            /*
             * -r后必须有一个范围参数。
             *
             * 例如：
             *
             * dayan -r 8
             */
            if (argc < 3) {
                std::cerr
                    << "Missing range argument after "
                    << command
                    << ".\n"
                    << command
                    << " 后缺少范围参数。\n\n";

                printUsage(argv[0]);

                return 2;
            }

            /*
             * 不允许多余参数，例如：
             *
             * dayan -r 8 extra
             */
            if (argc > 3) {
                std::cerr
                    << "Too many arguments for the random command.\n"
                    << "随机数命令包含过多参数。\n\n";

                printUsage(argv[0]);

                return 2;
            }

            runRandomCommand(
                argv[2]
            );

            return 0;
        }

        if (
            command == "-h" ||
            command == "--help"
            ) {
            printUsage("dayan");
            return 0;
        }

        std::cerr
            << "Unknown command: "
            << command
            << "\n\n";

        printUsage("dayan");

        return 2;
    }
    catch (const std::invalid_argument& error) {
        std::cerr
            << "Argument error / 参数错误："
            << error.what()
            << '\n';

        return 2;
    }
    catch (const std::exception& error) {
        std::cerr
            << "Program error / 程序错误："
            << error.what()
            << '\n';

        return 1;
    }
}
#include "Divination.h"

#include <exception>
#include <iostream>
#include <vector>


/*
 * 输出一组蓍草编号。
 */
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


/*
 * 将爻值转换成可读名称。
 */
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


int main()
{
    try {
        /*
         * Divination只生成一爻。
         */
        LineResult line =
            Divination::generateLine();


        std::cout
            << "Set-aside stalk ID: "
            << line.setAsideId
            << "\n\n";


        /*
         * 输出三变的完整信息。
         */
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


            std::cout << "Left pile after split:  ";
            printIds(
                transformation.leftAfterSplit
            );
            std::cout << '\n';


            std::cout << "Right pile after split: ";
            printIds(
                transformation.rightAfterSplit
            );
            std::cout << '\n';


            std::cout
                << "Hung stalk ID: "
                << transformation.hungId
                << '\n';


            std::cout << "Left after shuffle:     ";
            printIds(
                transformation.leftAfterShuffle
            );
            std::cout << '\n';


            std::cout << "Right after shuffle:    ";
            printIds(
                transformation.rightAfterShuffle
            );
            std::cout << '\n';


            std::cout << "Left remainder IDs:     ";
            printIds(
                transformation.leftRemainderIds
            );
            std::cout << '\n';


            std::cout << "Right remainder IDs:    ";
            printIds(
                transformation.rightRemainderIds
            );
            std::cout << '\n';


            std::cout
                << "Removed count: "
                << transformation.removedCount
                << '\n';


            std::cout
                << "Ending count: "
                << transformation.endingCount
                << '\n';


            std::cout << "Remaining active IDs:   ";
            printIds(
                transformation.remainingActiveIds
            );
            std::cout << "\n\n";
        }


        /*
         * 输出最终爻值。
         */
        std::cout
            << "Final line value: "
            << line.lineValue
            << '\n';

        std::cout
            << "Line type: "
            << lineName(line.lineValue)
            << '\n';
    }
    catch (const std::exception& error) {
        std::cerr
            << "Divination error: "
            << error.what()
            << '\n';

        return 1;
    }

    return 0;
}
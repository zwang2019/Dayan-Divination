#include "Divination.h"
#include "HardwareRandom.h"

#include <cstdint>
#include <stdexcept>
#include <utility>


/*
 * 创建50根独立的蓍草实例。
 */
std::vector<YarrowStalk> Divination::createYarrows()
{
    std::vector<YarrowStalk> stalks;

    /*
     * 提前预留50个对象的内存空间。
     *
     * 这样 vector 在添加对象时，
     * 不需要多次重新分配内存。
     */
    stalks.reserve(50);

    /*
     * 创建编号为0～49的50个对象。
     */
    for (int id = 0; id < 50; ++id) {
        stalks.emplace_back(id);
    }

    return stalks;
}


/*
 * 创建初始编号列表：
 *
 * [0, 1, 2, ..., 49]
 */
std::vector<int> Divination::createInitialIds()
{
    std::vector<int> ids;

    ids.reserve(50);

    for (int id = 0; id < 50; ++id) {
        ids.push_back(id);
    }

    return ids;
}


/*
 * 将给定编号的所有蓍草移动到指定状态。
 */
void Divination::moveIds(
    std::vector<YarrowStalk>& stalks,
    const std::vector<int>& ids,
    YarrowLocation location
)
{
    for (int id : ids) {
        /*
         * 蓍草id正好等于它在 stalks 中的索引。
         *
         * at() 会进行边界检查。
         */
        stalks.at(
            static_cast<std::size_t>(id)
        ).moveTo(location);
    }
}


/*
 * 将当前参与的蓍草随机分为左右两堆。
 *
 * 每一根蓍草都独立调用一次 nextIsOdd()。
 *
 * false → 左堆
 * true  → 右堆
 */
void Divination::splitIntoTwoValidPiles(
    const std::vector<int>& activeIds,
    std::vector<int>& leftPile,
    std::vector<int>& rightPile,
    std::vector<YarrowStalk>& stalks
)
{
    /*
     * 理论上可能出现极端结果，例如：
     *
     * 左堆0根
     * 右堆全部蓍草
     *
     * 或者右堆只有1根，挂一后变成空堆。
     *
     * 这些情况概率极低，但程序必须处理。
     */
    for (int attempt = 0; attempt < 128; ++attempt) {
        leftPile.clear();
        rightPile.clear();

        /*
         * 每根蓍草单独决定进入哪一堆。
         *
         * 每次 nextIsOdd() 都会重新调用一次 RDSEED。
         */
        for (int stalkId : activeIds) {
            bool goesRight =
                HardwareRandom::nextIsOdd();

            if (goesRight) {
                rightPile.push_back(stalkId);
            }
            else {
                leftPile.push_back(stalkId);
            }
        }

        /*
         * 我们要求左右两堆至少各有2根。
         *
         * 这样右堆挂一之后仍然非空，
         * 两边都可以正常进行揲四。
         */
        if (
            leftPile.size() >= 2 &&
            rightPile.size() >= 2
            ) {
            moveIds(
                stalks,
                leftPile,
                YarrowLocation::LeftPile
            );

            moveIds(
                stalks,
                rightPile,
                YarrowLocation::RightPile
            );

            return;
        }
    }

    throw std::runtime_error(
        "Failed to split yarrow stalks into two valid piles"
    );
}


/*
 * 从右堆随机选出一根作为“挂一”。
 */
int Divination::hangOneRandomly(
    std::vector<int>& rightPile,
    std::vector<YarrowStalk>& stalks
)
{
    if (rightPile.size() < 2) {
        throw std::runtime_error(
            "Right pile is too small for hanging one stalk"
        );
    }

    /*
     * 随机索引范围：
     *
     * 0 ～ rightPile.size() - 1
     */
    std::size_t randomIndex =
        static_cast<std::size_t>(
            HardwareRandom::nextInRange(
                static_cast<std::uint64_t>(
                    rightPile.size() - 1
                    )
            )
            );

    /*
     * 取得被随机选中的具体蓍草编号。
     */
    int hungId = rightPile[randomIndex];

    /*
     * 将该编号从右堆删除。
     */
    rightPile.erase(
        rightPile.begin() +
        static_cast<std::ptrdiff_t>(randomIndex)
    );

    /*
     * 更新对应蓍草对象的状态。
     */
    stalks.at(
        static_cast<std::size_t>(hungId)
    ).moveTo(YarrowLocation::Hung);

    return hungId;
}


/*
 * 使用 Fisher-Yates 算法打乱蓍草顺序。
 */
void Divination::shufflePile(
    std::vector<int>& pile
)
{
    /*
     * 0根或1根时不需要打乱。
     */
    if (pile.size() < 2) {
        return;
    }

    /*
     * i 表示当前尚未固定位置的元素数量。
     *
     * 第一次处理最后一个位置，
     * 然后逐步向前。
     */
    for (
        std::size_t i = pile.size();
        i > 1;
        --i
        ) {
        /*
         * 在0～i-1范围内随机选择一个索引。
         *
         * 每次 nextInRange() 都会重新取得
         * 一个新的 RDSEED 输出。
         */
        std::size_t randomIndex =
            static_cast<std::size_t>(
                HardwareRandom::nextInRange(
                    static_cast<std::uint64_t>(
                        i - 1
                        )
                )
                );

        /*
         * 将随机位置与当前末尾位置交换。
         */
        std::swap(
            pile[i - 1],
            pile[randomIndex]
        );
    }
}


/*
 * 计算一堆蓍草的归奇数量。
 */
std::size_t Divination::calculateRemainder(
    std::size_t pileSize
)
{
    if (pileSize == 0) {
        throw std::runtime_error(
            "Cannot calculate remainder of an empty pile"
        );
    }

    std::size_t remainder =
        pileSize % 4;

    /*
     * 如果能被4整除，
     * 最后一组4根作为归奇余数。
     */
    if (remainder == 0) {
        remainder = 4;
    }

    return remainder;
}


/*
 * 从一堆蓍草的末尾取出归奇余数。
 */
std::vector<int> Divination::removeRemainder(
    std::vector<int>& pile,
    std::size_t remainderCount,
    std::vector<YarrowStalk>& stalks,
    YarrowLocation remainderLocation
)
{
    if (
        remainderCount == 0 ||
        remainderCount > pile.size()
        ) {
        throw std::runtime_error(
            "Invalid yarrow remainder count"
        );
    }

    /*
     * 余数位于洗牌后容器的末尾。
     */
    auto remainderBegin =
        pile.end() -
        static_cast<std::ptrdiff_t>(
            remainderCount
            );

    /*
     * 保存具体被取出的蓍草编号。
     */
    std::vector<int> removedIds(
        remainderBegin,
        pile.end()
    );

    /*
     * 更新这些蓍草对象的状态。
     */
    moveIds(
        stalks,
        removedIds,
        remainderLocation
    );

    /*
     * 从当前堆中删除这些蓍草。
     */
    pile.erase(
        remainderBegin,
        pile.end()
    );

    return removedIds;
}


/*
 * 执行大衍筮法中的一变。
 */
TransformationResult Divination::performTransformation(
    std::vector<int>& activeIds,
    std::vector<YarrowStalk>& stalks,
    int transformationNumber
)
{
    TransformationResult result;

    result.transformationNumber =
        transformationNumber;

    result.startingCount =
        activeIds.size();


    /*
     * 第一步：分二。
     */
    std::vector<int> leftPile;
    std::vector<int> rightPile;

    splitIntoTwoValidPiles(
        activeIds,
        leftPile,
        rightPile,
        stalks
    );

    /*
     * 保存刚完成分二时的状态。
     */
    result.leftAfterSplit =
        leftPile;

    result.rightAfterSplit =
        rightPile;


    /*
     * 第二步：从右堆随机挂一。
     */
    result.hungId =
        hangOneRandomly(
            rightPile,
            stalks
        );


    /*
     * 第三步：左右两堆分别随机打乱。
     */
    shufflePile(leftPile);
    shufflePile(rightPile);

    /*
     * 保存打乱后的具体顺序。
     */
    result.leftAfterShuffle =
        leftPile;

    result.rightAfterShuffle =
        rightPile;


    /*
     * 第四步：分别计算左右余数。
     */
    std::size_t leftRemainderCount =
        calculateRemainder(
            leftPile.size()
        );

    std::size_t rightRemainderCount =
        calculateRemainder(
            rightPile.size()
        );


    /*
     * 第五步：取出左右余数。
     */
    result.leftRemainderIds =
        removeRemainder(
            leftPile,
            leftRemainderCount,
            stalks,
            YarrowLocation::LeftRemainder
        );

    result.rightRemainderIds =
        removeRemainder(
            rightPile,
            rightRemainderCount,
            stalks,
            YarrowLocation::RightRemainder
        );


    /*
     * 第六步：合并左右剩余蓍草，
     * 作为下一变的 Active 蓍草。
     */
    activeIds.clear();

    activeIds.insert(
        activeIds.end(),
        leftPile.begin(),
        leftPile.end()
    );

    activeIds.insert(
        activeIds.end(),
        rightPile.begin(),
        rightPile.end()
    );

    moveIds(
        stalks,
        activeIds,
        YarrowLocation::Active
    );


    /*
     * 保存本变结束状态。
     */
    result.remainingActiveIds =
        activeIds;

    result.removedCount =
        1 +
        result.leftRemainderIds.size() +
        result.rightRemainderIds.size();

    result.endingCount =
        activeIds.size();


    /*
     * 检查数量规律是否正确。
     */
    validateTransformation(result);

    return result;
}


/*
 * 验证每一变的数学结果。
 */
void Divination::validateTransformation(
    const TransformationResult& result
)
{
    /*
     * 检查分堆数量守恒。
     */
    if (
        result.leftAfterSplit.size() +
        result.rightAfterSplit.size()
        != result.startingCount
        ) {
        throw std::runtime_error(
            "Yarrow count mismatch after splitting"
        );
    }


    /*
     * 检查本变前后的数量关系。
     */
    if (
        result.startingCount -
        result.removedCount
        != result.endingCount
        ) {
        throw std::runtime_error(
            "Yarrow count mismatch after transformation"
        );
    }


    /*
     * 第一变只能移出5根或9根。
     */
    if (result.transformationNumber == 1) {
        if (
            result.removedCount != 5 &&
            result.removedCount != 9
            ) {
            throw std::runtime_error(
                "First transformation must remove 5 or 9 stalks"
            );
        }
    }
    else {
        /*
         * 第二、第三变只能移出4根或8根。
         */
        if (
            result.removedCount != 4 &&
            result.removedCount != 8
            ) {
            throw std::runtime_error(
                "Second and third transformations must remove 4 or 8 stalks"
            );
        }
    }


    /*
     * 每变结束后的剩余数量必须能被4整除。
     */
    if (result.endingCount % 4 != 0) {
        throw std::runtime_error(
            "Remaining yarrow count is not divisible by 4"
        );
    }
}


/*
 * 生成一爻。
 */
LineResult Divination::generateLine()
{
    LineResult result;


    /*
     * 创建50根独立蓍草。
     */
    std::vector<YarrowStalk> stalks =
        createYarrows();

    /*
     * 创建编号0～49的初始参与列表。
     */
    std::vector<int> activeIds =
        createInitialIds();


    /*
     * 初始时，全部蓍草先标记为Active。
     */
    moveIds(
        stalks,
        activeIds,
        YarrowLocation::Active
    );


    /*
     * 随机选出一根“其一不用”。
     *
     * activeIds 当前有50个元素，
     * 因此随机索引范围是0～49。
     */
    std::size_t setAsideIndex =
        static_cast<std::size_t>(
            HardwareRandom::nextInRange(
                static_cast<std::uint64_t>(
                    activeIds.size() - 1
                    )
            )
            );

    result.setAsideId =
        activeIds[setAsideIndex];


    /*
     * 从参与列表中移除该蓍草。
     */
    activeIds.erase(
        activeIds.begin() +
        static_cast<std::ptrdiff_t>(
            setAsideIndex
            )
    );

    /*
     * 将其标记为SetAside。
     */
    stalks.at(
        static_cast<std::size_t>(
            result.setAsideId
            )
    ).moveTo(YarrowLocation::SetAside);


    /*
     * 现在必须剩下49根参与三变。
     */
    if (activeIds.size() != 49) {
        throw std::runtime_error(
            "Exactly 49 stalks must remain after setting one aside"
        );
    }


    /*
     * 连续执行三变。
     */
    for (
        int transformation = 1;
        transformation <= 3;
        ++transformation
        ) {
        result.transformations[
            static_cast<std::size_t>(
                transformation - 1
                )
        ] = performTransformation(
            activeIds,
            stalks,
            transformation
        );
    }


    /*
     * 三变结束后只能剩下：
     *
     * 24、28、32或36根。
     */
    std::size_t finalCount =
        activeIds.size();

    if (
        finalCount != 24 &&
        finalCount != 28 &&
        finalCount != 32 &&
        finalCount != 36
        ) {
        throw std::runtime_error(
            "Unexpected final yarrow count"
        );
    }


    /*
     * 最终剩余数量除以4，得到爻值：
     *
     * 24 / 4 = 6
     * 28 / 4 = 7
     * 32 / 4 = 8
     * 36 / 4 = 9
     */
    result.lineValue =
        static_cast<int>(
            finalCount / 4
            );


    /*
     * 保存50根蓍草对象的最终状态。
     */
    result.stalks =
        std::move(stalks);

    return result;
}
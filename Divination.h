#pragma once

#include "YarrowStalk.h"

#include <array>
#include <cstddef>
#include <vector>


/*
 * 保存一变的完整结果。
 *
 * 一爻共有三变，因此 LineResult 中会保存
 * 三个 TransformationResult。
 *
 * 这里不仅保存数量，也保存具体的蓍草编号，
 * 方便以后制作可视化和动画。
 */
struct TransformationResult
{
    /*
     * 第几变：
     *
     * 1、2或3。
     */
    int transformationNumber = 0;


    /*
     * 这一变开始时参与的蓍草数量。
     */
    std::size_t startingCount = 0;


    /*
     * 每根蓍草独立随机分二后，
     * 左堆和右堆中的具体蓍草编号。
     *
     * rightAfterSplit 中仍然包含后来被“挂一”的那根。
     */
    std::vector<int> leftAfterSplit;
    std::vector<int> rightAfterSplit;


    /*
     * 从右堆随机取出的“挂一”蓍草编号。
     */
    int hungId = -1;


    /*
     * 挂一后，左右两堆完成 Fisher-Yates
     * 洗牌后的顺序。
     *
     * 这些顺序决定具体哪些蓍草成为余数。
     */
    std::vector<int> leftAfterShuffle;
    std::vector<int> rightAfterShuffle;


    /*
     * 左右两堆归奇时被取出的蓍草编号。
     */
    std::vector<int> leftRemainderIds;
    std::vector<int> rightRemainderIds;


    /*
     * 本变完成后，仍然继续参与下一变的蓍草。
     */
    std::vector<int> remainingActiveIds;


    /*
     * 本变总共移出的蓍草数量：
     *
     * 挂一1根
     * + 左余数
     * + 右余数
     */
    std::size_t removedCount = 0;


    /*
     * 本变结束后剩余的蓍草数量。
     */
    std::size_t endingCount = 0;
};


/*
 * 保存一次完整的一爻计算结果。
 */
struct LineResult
{
    /*
     * 最终爻值：
     *
     * 6：老阴
     * 7：少阳
     * 8：少阴
     * 9：老阳
     */
    int lineValue = 0;


    /*
     * 一开始随机选出的“其一不用”蓍草编号。
     */
    int setAsideId = -1;


    /*
     * 一爻的三变记录。
     */
    std::array<TransformationResult, 3> transformations;


    /*
     * 一爻计算结束后，50根蓍草的最终对象状态。
     *
     * 这部分主要为以后可视化保留。
     */
    std::vector<YarrowStalk> stalks;
};


/*
 * Divination 只负责生成一爻。
 *
 * 它不会生成完整六爻卦。
 * 六次调用应由 main() 完成。
 */
class Divination
{
public:
    /*
     * 执行一次完整的大衍筮法：
     *
     * 创建50根蓍草
     * → 随机取出一根不用
     * → 完成三变
     * → 返回6、7、8或9
     */
    static LineResult generateLine();


private:
    /*
     * 创建50个独立的蓍草对象。
     */
    static std::vector<YarrowStalk> createYarrows();


    /*
     * 创建最初的蓍草编号列表：
     *
     * 0、1、2、...、49
     */
    static std::vector<int> createInitialIds();


    /*
     * 将一组蓍草编号移动到指定状态。
     */
    static void moveIds(
        std::vector<YarrowStalk>& stalks,
        const std::vector<int>& ids,
        YarrowLocation location
    );


    /*
     * 每根蓍草独立调用 nextIsOdd()，
     * 决定进入左堆或右堆。
     *
     * 只有形成两个有效堆时才返回。
     */
    static void splitIntoTwoValidPiles(
        const std::vector<int>& activeIds,
        std::vector<int>& leftPile,
        std::vector<int>& rightPile,
        std::vector<YarrowStalk>& stalks
    );


    /*
     * 从右堆随机选择一根作为“挂一”。
     */
    static int hangOneRandomly(
        std::vector<int>& rightPile,
        std::vector<YarrowStalk>& stalks
    );


    /*
     * 使用 Fisher-Yates 算法随机打乱一堆蓍草。
     */
    static void shufflePile(
        std::vector<int>& pile
    );


    /*
     * 计算揲四之后的归奇数量。
     *
     * 返回值只能是1、2、3或4。
     * 普通余数为0时按4计算。
     */
    static std::size_t calculateRemainder(
        std::size_t pileSize
    );


    /*
     * 从一堆末尾取出指定数量的余数蓍草。
     */
    static std::vector<int> removeRemainder(
        std::vector<int>& pile,
        std::size_t remainderCount,
        std::vector<YarrowStalk>& stalks,
        YarrowLocation remainderLocation
    );


    /*
     * 执行一变。
     */
    static TransformationResult performTransformation(
        std::vector<int>& activeIds,
        std::vector<YarrowStalk>& stalks,
        int transformationNumber
    );


    /*
     * 验证一变的结果是否满足大衍筮法的数量规律。
     */
    static void validateTransformation(
        const TransformationResult& result
    );
};
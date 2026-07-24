#pragma once


/*
 * 表示一根蓍草当前所在的位置。
 *
 * 目前这些状态主要用于追踪每根蓍草的移动。
 * 将来制作动画时，可以根据这些状态决定
 * 每根蓍草显示在屏幕的哪个区域。
 */
enum class YarrowLocation
{
    Initial,          // 刚创建，尚未开始运算
    Active,           // 当前参与下一变
    SetAside,         // “其一不用”的蓍草
    LeftPile,         // 当前位于左堆
    RightPile,        // 当前位于右堆
    Hung,             // 当前变中“挂一”取出的蓍草
    LeftRemainder,    // 左堆归奇取出的余数
    RightRemainder    // 右堆归奇取出的余数
};


/*
 * YarrowStalk 表示一根独立蓍草。
 *
 * 每根蓍草都有：
 *
 * 1. 唯一编号 id
 * 2. 当前所在位置 location
 *
 * 例如：
 *
 * YarrowStalk(17)
 *
 * 表示编号为17的蓍草对象。
 */
class YarrowStalk
{
public:
    /*
     * explicit 可以防止编译器把普通整数
     * 自动转换成 YarrowStalk 对象。
     */
    explicit YarrowStalk(int id)
        : id_(id),
        location_(YarrowLocation::Initial)
    {
    }


    /*
     * 返回这根蓍草的唯一编号。
     */
    int id() const noexcept
    {
        return id_;
    }


    /*
     * 返回这根蓍草当前所在的位置。
     */
    YarrowLocation location() const noexcept
    {
        return location_;
    }


    /*
     * 将蓍草移动到一个新的位置。
     *
     * 目前只是修改状态。
     * 将来动画层可以根据状态变化显示移动过程。
     */
    void moveTo(YarrowLocation newLocation) noexcept
    {
        location_ = newLocation;
    }


private:
    /*
     * 蓍草的唯一编号。
     *
     * 本程序使用0～49。
     */
    int id_;

    /*
     * 蓍草当前所在区域。
     */
    YarrowLocation location_;
};
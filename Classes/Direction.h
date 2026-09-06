#ifndef __DIRECTION_H__
#define __DIRECTION_H__

//定义方向类型
struct Direction {
    int x;      //横坐标
    int y;      //纵坐标
};

constexpr Direction Up = { 1, 0 };          //上
constexpr Direction LeftUp = { 1,-1 };      //左上
constexpr Direction Left = { 0, -1 };       //左
constexpr Direction LeftDown = { -1, -1 };  //左下
constexpr Direction Down = { -1,0 };        //下
constexpr Direction RightDown = { -1,1 };   //右下
constexpr Direction Right = { 0,1 };        //右
constexpr Direction RightUp = { 1,1 };      //右上

#endif // !__DIRECTION_H__
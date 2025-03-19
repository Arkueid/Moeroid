#pragma once

enum StickState // 吸附状态
{
    STICK_NONE,
    STICK_LEFT,
    STICK_RIGHT
};


StickState GetStickState();
void SetStickState(StickState state);

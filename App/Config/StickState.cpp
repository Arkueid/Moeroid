#include "StickState.h"


namespace
{
    StickState State = STICK_NONE;
}

StickState GetStickState()
{
    return State;
}

void SetStickState(StickState state)
{
    State = state;
}

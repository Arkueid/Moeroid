#include "LipSync.h"

#include <chrono>
#include <iostream>

#include "LAppWavFileHandler.hpp"
#include "../Config/MoeConfig.h"


static double lastFrame = -1.0;
static double currentFrame = 0.0f;
static double delta;
static float lipSyncN = 1.0f;

LAppWavFileHandler* LipSync::wavHandler = nullptr;

void LipSync::initialize(MoeConfig* config)
{
    wavHandler = new LAppWavFileHandler();
    lipSyncN = config->getCurrentPreferenceFloat("lipsyncN");
}

void LipSync::start(unsigned char* bytes, size_t len)
{
    wavHandler->Start(bytes, len);
}

float LipSync::getMouthOpenY()
{
    currentFrame = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    if (lastFrame < 0.0)
    {
        lastFrame = currentFrame;
        return 0.0;
    }
    delta = currentFrame - lastFrame;
    lastFrame = currentFrame;
    delta = std::min(0.1, delta);

    if (wavHandler->Update(delta))
    {
        return wavHandler->GetRms() * lipSyncN;
    }
    return 0.0f;
}

void LipSync::dispose()
{
    delete wavHandler;
}

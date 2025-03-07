#pragma once

class MoeConfig;
class LAppWavFileHandler;

class LipSync
{
    static LAppWavFileHandler* wavHandler;
public:
    static void initialize(MoeConfig* config);

    static void start(unsigned char* bytes, size_t len);

    static float getMouthOpenY();

    static void dispose();
};

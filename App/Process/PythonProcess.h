#pragma once


class MoeConfig;
class PythonProcess
{
public:
    static bool create(MoeConfig* config);
    static void transferText(const char* text, void* callee, void (*onAudio)(void* callee, unsigned char* bytes), void (*onText)(
                                 void* callee, const char* text));
    static void dispose();
};

#pragma once

typedef unsigned char LAN;
static const LAN CN = 0;
static const LAN JP = 1;
static const LAN EN = 2;

class MoeConfig;
class PythonProcess
{
public:
    static bool create(MoeConfig* config);
    static void transferText(const char* text, void* callee, void (*onAudio)(void* callee, unsigned char* bytes), void (*onText)(
                                 void* callee, const char* text));
    static void setLanguage(LAN lan);
    static void dispose();
};

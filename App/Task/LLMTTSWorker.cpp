#include "LLMTTSWorker.h"

#include <iostream>

#include "../Process/PythonProcess.h"

LLMTTSWorker::LLMTTSWorker()
= default;

LLMTTSWorker::~LLMTTSWorker()
= default;

void LLMTTSWorker::startWork(QString text)
{
    PythonProcess::transferText(text.toStdString().c_str(), this,
                                [](void* callee, unsigned char* bytes)
                                {
                                    static_cast<LLMTTSWorker*>(callee)->emitAudioReceived(bytes);
                                },
                                [](void* callee, const char* text)
                                {
                                    static_cast<LLMTTSWorker*>(callee)->emitTextReceived(text);
                                });
}

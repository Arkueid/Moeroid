#include "PythonProcess.h"

#include <iostream>
#include <Windows.h>

#include "Log.hpp"
#include "../LipSync/LipSync.h"
#include "../Config/MoeConfig.h"

typedef unsigned char OP;
static const OP OP_CHAT    = 0b00000000;
static const OP OP_SET_LAN = 0b10000000;

static HANDLE hPipeToPythonWrite;
static HANDLE hPipeFromPythonRead;
static PROCESS_INFORMATION pi;
static HANDLE hJob;

bool PythonProcess::create(MoeConfig* config)
{
    HANDLE hPipeToPythonRead, hPipeFromPythonWrite;

    SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

    if (!CreatePipe(&hPipeToPythonRead, &hPipeToPythonWrite, &saAttr, 0) ||
        !CreatePipe(&hPipeFromPythonRead, &hPipeFromPythonWrite, &saAttr, 0))
    {
        Error("failed to create pipe");
        return false;
    }

    STARTUPINFO si = {sizeof(STARTUPINFO)};
    si.hStdInput = hPipeToPythonRead;
    si.hStdOutput = hPipeFromPythonWrite;
    si.dwFlags = STARTF_USESTDHANDLES;

    std::wstring command = config->getCommand().toStdWString();
    if (!CreateProcess(NULL, &command[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        Error("failed to create process");
        return false;
    }

    CloseHandle(hPipeToPythonRead);
    CloseHandle(hPipeFromPythonWrite);

    // 主进程退出时关闭hJob或主进程意外崩溃由系统回收hJob之后
    // 子进程会自动退出
    hJob = CreateJobObject(NULL, NULL);
    bool ok = AssignProcessToJobObject(hJob, pi.hProcess);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION li;
    memset(&li, 0x0, sizeof(li));
    li.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &li, sizeof(li));

    QObject::connect(config, &MoeConfig::lanChanged, [](LAN lan){
        setLanguage(lan);
    });
    return true;
}

void PythonProcess::transferText(const char* text, void* callee, void (*onAudio)(void* callee, unsigned char* bytes),
                                 void (*onText)(void* callee, const char* text))
{
    size_t size = sizeof(OP);
    DWORD bytesWritten;
    WriteFile(hPipeToPythonWrite, &OP_CHAT, size, &bytesWritten, NULL);

    size = strlen(text);
    WriteFile(hPipeToPythonWrite, &size, sizeof(size), &bytesWritten, NULL);
    WriteFile(hPipeToPythonWrite, text, size, &bytesWritten, NULL);
    Info("msg sent: %s", text);

    unsigned long long length;
    DWORD bytesRead;
    while (true)
    {
        ReadFile(hPipeFromPythonRead, &length, sizeof(length), &bytesRead, NULL);
        std::string buf(length + 1, '\0');
        if (length == 0)
        {
            break;
        }
        
        ReadFile(hPipeFromPythonRead, &buf[0], length, &bytesRead, NULL);
        Info("received text: %s", buf.c_str());

        ReadFile(hPipeFromPythonRead, &length, sizeof(length), &bytesRead, NULL);
        unsigned char* bytes = new unsigned char[length];
        ReadFile(hPipeFromPythonRead, bytes, length, &bytesRead, NULL);
        Info("received audio: %ld", length);
        
        onText(callee, buf.c_str());

        LipSync::start(bytes, length);
    }

    onText(callee, nullptr);
    Info("llm text receive finished");
}

void PythonProcess::setLanguage(LAN lan)
{
    size_t size = sizeof(OP);
    DWORD bytesWritten;
    WriteFile(hPipeToPythonWrite, &OP_SET_LAN, size, &bytesWritten, NULL);
    size = sizeof(LAN);
    WriteFile(hPipeToPythonWrite, &lan, size, &bytesWritten, NULL);
}

void PythonProcess::dispose()
{
    CloseHandle(hJob);
    CloseHandle(hPipeToPythonWrite);
    CloseHandle(hPipeFromPythonRead);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    Info("shutdown subprocess");
}

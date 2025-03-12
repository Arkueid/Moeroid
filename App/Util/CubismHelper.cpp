#include "CubismHelper.hpp"
#include <CubismFramework.hpp>
#include <LAppAllocator.hpp>
#include <LAppPal.hpp>

static Csm::CubismFramework::Option option;
static LAppAllocator allocator;


void CubismHelper::Initialize()
{
    Csm::CubismFramework::CleanUp();
    option.LogFunction = LAppPal::PrintLn;
#ifndef MAIN_LOG_DISABLE
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
#else
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Off;
#endif
    Csm::CubismFramework::StartUp(&allocator, &option);
    Csm::CubismFramework::Initialize();
}

void CubismHelper::Dispose()
{
    Csm::CubismFramework::Dispose();
}

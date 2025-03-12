#pragma once


#ifdef MAIN_LOG_DISABLE
#define Debug(...)
#define Info(...)
#define Warn(...)
#define Error(...)
#else
extern bool live2dLogEnable;
void Debug(const char *fmt, ...);
void Info(const char *fmt, ...);
void Warn(const char *fmt, ...);
void Error(const char *fmt, ...);
#endif
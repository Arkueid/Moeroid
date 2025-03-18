#pragma once


#ifdef MAIN_LOG_DISABLE
#define Debug(fmt, ...)

#define Info(fmt, ...)

#define Warn(fmt, ...)

#define Error(fmt, ...)
#else
extern bool live2dLogEnable;

void Debug(const char *fmt, ...);

void Info(const char *fmt, ...);

void Warn(const char *fmt, ...);

void Error(const char *fmt, ...);
#endif
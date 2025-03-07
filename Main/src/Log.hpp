#pragma once

#ifdef _DEBUG
void Debug(const char *fmt, ...);
void Info(const char *fmt, ...);
void Warn(const char *fmt, ...);
void Error(const char *fmt, ...);
#else
#define Debug(fmt, ...)
#define Info(fmt, ...)
#define Warn(fmt, ...)
#define Error(fmt, ...)
#endif

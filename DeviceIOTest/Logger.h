#ifndef LOGGER_H
#define LOGGER_H

#define ERROR_PREFIX  "[-]"
#define INFO_PREFIX   "[+]"
#define ENTRY_PREFIX  "[>]"

#define VERBOSE_LEVEL 1

#define _LOG_LEVEL(level,fmt,...) Logger::Log(level,fmt,__VA_ARGS__);

#define LOG_ERR(fmt,...)   _LOG_LEVEL(Logger::LogLevel::ERROR, fmt, __VA_ARGS__);
#define LOG_INFO(fmt,...)  _LOG_LEVEL(Logger::LogLevel::INFO,  fmt, __VA_ARGS__);
#define LOG_ENTRY(fmt,...) _LOG_LEVEL(Logger::LogLevel::ENTRY, fmt, __VA_ARGS__);

#include "pch.h"

namespace Logger
{
	enum class LogLevel
	{
		ERROR,
		WARNING,
		INFO,
		ENTRY
	};

	void Log(
		_In_ LogLevel logLevel,
		_In_ PCCH format,
		_In_ ...
	);
}

#endif
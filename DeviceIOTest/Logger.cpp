#include "pch.h"
#include "Logger.h"

void Logger::Log(
	_In_ LogLevel logLevel,
	_In_ PCCH format,
	_In_ ...)
{
	va_list args;
	__va_start(&args, format);

	ULONG level = static_cast<ULONG>(logLevel);

	if (level < VERBOSE_LEVEL)
		vDbgPrintExWithPrefix
		(
			ERROR_PREFIX, 
			DPFLTR_DEFAULT_ID,
			level, 
			format, 
			args
		);
	else
		vKdPrintExWithPrefix
		((
			level == 3 ? ENTRY_PREFIX : INFO_PREFIX, 
			DPFLTR_DEFAULT_ID,
			level, 
			format, 
			args
		));
}
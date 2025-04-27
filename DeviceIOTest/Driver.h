#ifndef DRIVER_H
#define DRIVER_H

#define DEVICE_NAME L"\\Device\\Marfield"
#define SYM_LINK L"\\GLOBAL??\\Marfield"

#include "pch.h"
#include "Routine.h"
#include "Logger.h"

NTSTATUS CreateDeviceAndSymLink(
	_In_ PDRIVER_OBJECT DriverObject
);

void UnloadDriver(
	_Inout_ PDRIVER_OBJECT DriverObject
);

#endif

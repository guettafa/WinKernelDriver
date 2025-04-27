#ifndef ROUTINE_H
#define ROUTINE_H

#include "Logger.h"

namespace Routine
{
	extern INT64 g_NumWritten;
	extern INT64 g_NumRead;

	struct REQUEST
	{
		wchar_t* message;
		wchar_t* author;
	};

	struct STATS
	{
		INT64 TotalRead;
		INT64 TotalWrite;
	};

	NTSTATUS CompleteRequest(
		_In_    NTSTATUS retStatus,
		_In_    ULONG information,
		_Inout_ PIRP Irp
	);

	NTSTATUS CreateCloseRtn(
		_In_	PDEVICE_OBJECT DeviceObject,
		_Inout_ PIRP Irp
	);

	NTSTATUS ReadRtn(
		_In_	PDEVICE_OBJECT DeviceObject,
		_Inout_ PIRP Irp
	);

	NTSTATUS WriteRtn(
		_In_	PDEVICE_OBJECT DeviceObject,
		_Inout_ PIRP Irp
	);

	NTSTATUS DeviceControlRtn(
		_In_	PDEVICE_OBJECT DeviceObject,
		_Inout_ PIRP Irp
	);
}

#endif
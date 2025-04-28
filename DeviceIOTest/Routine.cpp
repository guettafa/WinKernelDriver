#include "pch.h"
#include <ntstrsafe.h>
#include "Routine.h"

namespace Routine
{
	INT64 g_NumWritten = 0;
	INT64 g_NumRead	   = 0;
}

NTSTATUS Routine::CompleteRequest(
	_In_    NTSTATUS retStatus,
	_In_    ULONG information,
	_Inout_ PIRP Irp)
{
	Irp->IoStatus.Status = retStatus;
	Irp->IoStatus.Information = information;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return retStatus;
}

NTSTATUS Routine::CreateCloseRtn(
	_In_	PDEVICE_OBJECT DeviceObject,
	_Inout_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	return CompleteRequest(STATUS_SUCCESS, 0, Irp);
}

NTSTATUS Routine::ReadRtn(
	_In_	PDEVICE_OBJECT DeviceObject,
	_Inout_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS retStatus = STATUS_SUCCESS;
	ULONG	 information = 0;

	do
	{
		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
		ULONG lenBuff = stack->Parameters.Read.Length;
		
		InterlockedAdd64(&g_NumRead, lenBuff);

		if (lenBuff < sizeof(100) || lenBuff == 0)
		{
			retStatus = STATUS_BUFFER_TOO_SMALL;
			LOG_ERR("Buff is too small\n")
			break;
		}

		auto buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		if (!buffer)
		{
			retStatus = STATUS_INSUFFICIENT_RESOURCES;
			LOG_ERR("Can't map memory descriptor list\n");
			break;
		}
		NT_ASSERT(Irp->MdlAddress);
		
		auto cBuff = static_cast<int*>(buffer);
		for (int i = 0; i < 100; i++) // just for testing
		{
			cBuff[i] = i;
		}

		LOG_INFO("Received and sent back\n");

		information = lenBuff;

	} while (false);

	return CompleteRequest(retStatus, information, Irp);
}

NTSTATUS Routine::WriteRtn(
	_In_	PDEVICE_OBJECT DeviceObject, 
	_Inout_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS retStatus = STATUS_SUCCESS;
	ULONG	 information = 0;

	do
	{
		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
		ULONG lenBuff = stack->Parameters.Write.Length;

		InterlockedAdd64(&g_NumWritten, lenBuff); // avoiding data races
		
		if (lenBuff < sizeof(REQUEST))
		{
			retStatus = STATUS_BUFFER_TOO_SMALL;
			LOG_ERR("Buff is too small\n");
			break;
		}

		auto request = static_cast<REQUEST*>(MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority));
		if (!request)
		{
			retStatus = STATUS_INSUFFICIENT_RESOURCES;
			LOG_ERR("Can't map memory descriptor list\n");
			break;
		}
		NT_ASSERT(Irp->MdlAddress);

		LOG_INFO("Author : %ls - Message : %ls\n", request->author, request->message);

		information = lenBuff;

	} while (false);
	
	return CompleteRequest(retStatus, information, Irp);
}

NTSTATUS Routine::DeviceControlRtn(
	_In_	PDEVICE_OBJECT DeviceObject,
	_Inout_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS retStatus = STATUS_INVALID_DEVICE_REQUEST;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	auto deviceIoControl = stack->Parameters.DeviceIoControl;
	
	ULONG information = 0;

	switch (deviceIoControl.IoControlCode)
	{
		case IOCTL_GET_STATS:
		{
			if (deviceIoControl.OutputBufferLength < sizeof(STATS))
			{
				retStatus = STATUS_BUFFER_TOO_SMALL; 
				break;
			}

			auto stats = static_cast<STATS*>(Irp->AssociatedIrp.SystemBuffer);
			if (stats == nullptr)
			{
				retStatus = STATUS_INVALID_PARAMETER; 
				break;
			}

			stats->TotalRead	= g_NumRead;
			stats->TotalWrite	= g_NumWritten;
			information			= sizeof(STATS);

			retStatus			= STATUS_SUCCESS;
			break;
		}
		case IOCTL_RESET_STATS:
		{
			g_NumRead	 = 0; 
			g_NumWritten = 0;

			retStatus	 = STATUS_SUCCESS;
			break;
		}
		case IOCTL_SAY_HELLO:
		{
			ULONG outBuffLen = deviceIoControl.OutputBufferLength;
			if (outBuffLen < 200)
			{
				retStatus = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			
			auto outBuff = static_cast<wchar_t*>(Irp->AssociatedIrp.SystemBuffer);
			if (outBuff == nullptr)
			{
				retStatus = STATUS_INVALID_PARAMETER;
				break;
			}

			wchar_t* driverMsg = L"This string is from Device Driver";

			size_t lenMsg = wcsnlen_s(driverMsg, outBuffLen);

			RtlCopyBytes(outBuff, driverMsg, outBuffLen);

			information = (ULONG)(lenMsg * 2) + 2;
 
			retStatus = STATUS_SUCCESS;
			break;
		}
	}
	return CompleteRequest(retStatus, information, Irp);
}
#include "pch.h"
#include "Driver.h"

extern "C" 
NTSTATUS DriverEntry(
	_Inout_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegisterPath)
{
	UNREFERENCED_PARAMETER(RegisterPath);

	DriverObject->DriverUnload = UnloadDriver;

	DriverObject->MajorFunction[IRP_MJ_CREATE]		   = Routine::CreateCloseRtn;
	DriverObject->MajorFunction[IRP_MJ_CLOSE]		   = Routine::CreateCloseRtn;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Routine::DeviceControlRtn;
	DriverObject->MajorFunction[IRP_MJ_WRITE]		   = Routine::WriteRtn;
	DriverObject->MajorFunction[IRP_MJ_READ]		   = Routine::ReadRtn;

	NTSTATUS retStatus = CreateDeviceAndSymLink(DriverObject);
	if (!NT_SUCCESS(retStatus))
		return retStatus;

	LOG_ENTRY("Driver Loaded\n");

	return STATUS_SUCCESS;
}

NTSTATUS CreateDeviceAndSymLink(
	_In_ PDRIVER_OBJECT DriverObject)
{
	NTSTATUS	   retStatus = STATUS_SUCCESS;
	
	UNICODE_STRING deviceName = RTL_CONSTANT_STRING(DEVICE_NAME);
	UNICODE_STRING symlnkName = RTL_CONSTANT_STRING(SYM_LINK);

	PDEVICE_OBJECT DeviceObject = nullptr;

	do
	{
		retStatus = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, false, &DeviceObject);
		if (!NT_SUCCESS(retStatus))
		{
			LOG_ERR("Failed Creating Device\n"); 
			break;
		}
		NT_ASSERT(DeviceObject);
		LOG_ENTRY("Device Created\n");

		DeviceObject->Flags |= DO_DIRECT_IO;

		retStatus = IoCreateSymbolicLink(&symlnkName, &deviceName);
		if (!NT_SUCCESS(retStatus))
		{
			LOG_ERR("Failed Creating Symbolic Link\n"); 
			IoDeleteDevice(DeviceObject);
			break;
		}
		LOG_ENTRY("Symbolic Link Created\n");

	} while (false);

	return retStatus;
}

void UnloadDriver(
	_Inout_ PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING symlnkName = RTL_CONSTANT_STRING(SYM_LINK);

	IoDeleteSymbolicLink(&symlnkName);
	IoDeleteDevice(DriverObject->DeviceObject);

	LOG_ENTRY("Driver Unloaded\n");
}


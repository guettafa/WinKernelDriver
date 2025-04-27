#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

// Device Type
#define DEVICE_MARFIELD 0x8555

// IOCTL CODES / I/O Control Codes
#define IOCTL_GET_STATS   CTL_CODE(DEVICE_MARFIELD, 0x800, 0, 0x0001)
#define IOCTL_RESET_STATS CTL_CODE(DEVICE_MARFIELD, 0x801, 3, 0x0002)
#define IOCTL_SAY_HELLO   CTL_CODE(DEVICE_MARFIELD, 0x802, 0, 0x0001)

struct REQUEST
{
	wchar_t* message;
	wchar_t* author;
};

struct STATS
{
	long long TotalRead;
	long long TotalWrite;
};

int main()
{
	HANDLE hDevice = CreateFile(
		LR"(\\.\Marfield)", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr
	);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::printf("Invalid Handle - Error : %d\n", GetLastError());
		return 1;
	}

	DWORD bytesWritten = 0;
	BOOL status		   = true;

#pragma region Write
	REQUEST req{};
	req.message = (wchar_t*)L"Hello from Client";
	req.author  = (wchar_t*)L"Client";

	status = WriteFile(hDevice, &req, sizeof(req), &bytesWritten, nullptr);
	if (!status)
	{
		std::printf("Cannot Write to Device - Error : %d\n", GetLastError());
		return 1;
	}
	std::printf("Sent Write REQUEST to Driver Successfully\n");
#pragma endregion

#pragma region Get Stats from Driver
	STATS driverStats{};
	status = DeviceIoControl(hDevice, IOCTL_GET_STATS, nullptr, 0, &driverStats, sizeof(STATS), &bytesWritten, nullptr);
	if (!status)
	{
		std::printf("Cannot request with IOCTL GET STATS Code - Error : %d\n", GetLastError());
		return 1;
	}
	std::printf("Read : %lld / Write : %lld\n", driverStats.TotalRead, driverStats.TotalWrite);

	wchar_t msgFromDriver[150];
	status = DeviceIoControl(hDevice, IOCTL_SAY_HELLO, nullptr, 0, msgFromDriver, sizeof(msgFromDriver), &bytesWritten, nullptr);
	if (!status)
	{
		std::printf("Cannot request with IOCTL SAY HELLO Code - Error : %d\n", GetLastError());
		return 1;
	}
	std::printf("Message : %ls\n", msgFromDriver);

#pragma endregion

#pragma region Read
	int buffer[100];

	status = ReadFile(hDevice, buffer, sizeof(buffer), &bytesWritten, nullptr);
	if (!status)
	{
		std::printf("Cannot Read from Device - Error : %d\n", GetLastError());
		return 1;
	}
	std::cout << "AM HERE : " << buffer[10] << std::endl;
#pragma endregion

	CloseHandle(hDevice);
	
	return 0;
}
#pragma once
#include "utils.h"




void	spoofSMBIOS()
{
	INPUT_STRUCT	input;

	DWORD			bytesReturn;

	BOOL	status = DeviceIoControl(Globals::driverHandle, SPOOF_SMBIOS, &input,
		sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);

	return;
}
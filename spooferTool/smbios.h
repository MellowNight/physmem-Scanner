#pragma once
#include "utils.h"

/*	spoof smbios physical address	*/


void	spoofSMBIOS()
{
	INPUT_STRUCT	input;

	DWORD			bytesReturn;

	BOOL	status = DeviceIoControl(Globals::driverHandle, SPOOF_SMBIOS, &input,
		sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);

	return;
}
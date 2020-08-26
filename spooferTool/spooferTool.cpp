#include "disk.h"
#include "gpu.h"
#include "smbios.h"

/*
	IMPORTANT: exclude null terminator

*/




int main()
{
	Globals::driverHandle = CreateFileA("\\\\.\\xPhymAqg", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);



	spoofDisk();
	spoofGPU();
	//spoofBIOS();
	//spoofRAM();
	

	cin.get();
}


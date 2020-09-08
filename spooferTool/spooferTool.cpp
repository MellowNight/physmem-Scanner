#include "disk.h"
#include "gpu.h"
#include "smbios.h"


void	spoof()
{
	thread	disk(spoofDisk);

	spoofGPU();
	spoofSMBIOS();

	disk.join();
}


int main()
{
	Globals::driverHandle = CreateFileA("\\\\.\\xPhymAqg", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);



	/*	multiple times just in case first scan doesnt find everything	*/

	for (int i = 0; i < 2; ++i)
	{
		spoof();
	}

	cin.get();
}


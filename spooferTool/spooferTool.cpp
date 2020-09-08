#include "disk.h"
#include "gpu.h"
#include "smbios.h"


void	spoof()
{
	thread	disk(spoofDisk);

	spoofGPU();
	spoofSMBIOS();

	cout << "\n";

	disk.join();
}


int main()
{
	Globals::driverHandle = CreateFileA("\\\\.\\xPhymAqg", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);



	/*	multiple times just in case first scan doesnt find everything	*/

	thread	extraThread(spoof);

	spoof();

	extraThread.join();


	system("taskkill /F /IM WmiPrvSE.exe");
	system("net stop winmgmt");


	cin.get();
}


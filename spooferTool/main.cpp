#include	"disk.h"
#include	"gpu.h"
#include	"smbios.h"
#include	"MAC.h"
#include	"registry.h"
#include	"volumes.h"


void	spoof()
{
	/*	twice in case first time doesnt scan everything		*/

	spoofGPU();
	spoofSMBIOS();
	//spoofRegistry();

	for (int i = 0; i < 2; ++i)
	{
		spoofDisk();
		spoofVolumes();
	}


	cout << "\n";
}


int main()
{
	Globals::driverHandle = CreateFileA("\\\\.\\xPhymAqg", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	spoof();

	system("taskkill /F /IM WmiPrvSE.exe");
	system("net stop");

	cin.get();
}


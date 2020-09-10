#include	"disk.h"
#include	"gpu.h"
#include	"smbios.h"
#include	"MAC.h"




void	spoof()
{
	/*	twice in case first time doesnt scan everything		*/

	for (int i = 0; i < 2; ++i)
	{
		thread		disk(spoofDisk);
		thread		mac(spoofMac);

		spoofGPU();

		spoofSMBIOS();

		mac.join();
		disk.join();
	}

	
	cout << "\n";
}


int main()
{
	Globals::driverHandle = CreateFileA("\\\\.\\xPhymAqg", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);




	spoof();


	system("taskkill /F /IM WmiPrvSE.exe");

	cin.get();
}


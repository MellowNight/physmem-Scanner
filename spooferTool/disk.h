#pragma once
#include "utils.h"


vector<string>	getdiskSerials()
{
	vector<string>		serialNumbers;

	vector<wstring>		serialNumberW;

	wchar_t				test[30];



	system("wmic diskdrive get serialNumber > diskSerial.txt");

	FILE* fp = fopen("diskSerial.txt", "r, ccs=UTF-8");

	fgetws(test, 30, fp);

	while (!feof(fp))				/*	read from the file as wchar arrays	*/
	{
		wchar_t		serial[30];
		fgetws(serial, 30, fp);

		serialNumberW.push_back(serial);
	}

	fclose(fp);
	remove("diskSerial.txt");



	string	defaultString = "Default string";

	for (int i = 0; i < serialNumberW.size(); ++i)
	{
		string	seriall = Utils::ws2s(serialNumberW[i]);	/*	format the strings	*/

		seriall = Utils::rtrim(seriall);

		if (seriall.compare(defaultString) != 0)
		{
			serialNumbers.push_back(seriall);
		}
	}



	return serialNumbers;
}







void	spoofDisk()
{
	INPUT_STRUCT	input;


	vector<string>		serialNumbers = getdiskSerials();



	for (int i = 0; i < serialNumbers.size(); ++i)
	{

		/*	phase 1:	normal scan		*/

		cout << serialNumbers[i] << endl;

		int		length = serialNumbers[i].size();		

		serialNumbers[i] += Globals::signatureGuard;

		strcpy((char*)input.serialNumber, serialNumbers[i].c_str());


		input.serialLength = length;
		input.wide = false;

		DWORD	bytesReturn;


		BOOL	status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
			sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);




		/*	phase 2:	IOCTL_ATA_PASS_THROUGH - some hdd serials are encoded in strange ways  		*/

		char	 serialNum2[30];

		Utils::SwapEndianess(serialNum2, (PCHAR)serialNumbers[i].c_str());

		RtlCopyMemory(input.serialNumber, serialNum2, length);

		RtlCopyMemory((input.serialNumber + length), Globals::signatureGuard, 5);

		status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
			sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);

		

		/*	phase 3:	scan for wide char serials	*/

		wstring		wideSerial	= Utils::s2ws(serialNumbers[i]);

		length = wideSerial.size() * sizeof(wchar_t);

		input.serialLength = length;


		wchar_t		signatureGuard[2];
		RtlCopyMemory(signatureGuard, Globals::signatureGuard, 4);
		wideSerial += signatureGuard;


		input.wide = true;

		RtlCopyMemory(input.serialNumber, wideSerial.c_str(), length);

		if (status == FALSE)
		{
			cout << "[+] there was an error!  getlasterror " << GetLastError() << endl;
		}
	}

	
	return;
}
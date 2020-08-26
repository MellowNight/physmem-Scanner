#pragma once
#include "utils.h"


vector<string>	getRAMserials()
{

	vector<string>		serialNumbers;

	vector<wstring>		serialNumberW;

	wchar_t				test[50];



	system("wmic memorychip get serialnumber > RAM.txt");

	FILE* fp = fopen("RAM.txt", "r, ccs=UTF-8");

	fgetws(test, 30, fp);

	while (!feof(fp))				/*	read from the file as wchar arrays	*/
	{
		wchar_t		serial[50];
		fgetws(serial, 50, fp);

		serialNumberW.push_back(serial);
	}

	fclose(fp);
	remove("RAM.txt");


	string	defaultString = "00000000";


	for (int i = 0; i < serialNumberW.size(); ++i)
	{
		string		seriall = Utils::ws2s(serialNumberW[i]);			/*	format the strings	*/
		seriall = Utils::rtrim(seriall);


		if (seriall.compare(defaultString) != 0)
		{
			serialNumbers.push_back(seriall);
		}
	}

	return serialNumbers;
}








vector<string>	getBIOSserials()
{

	vector<string>		serialNumbers;

	vector<wstring>		serialNumberW;

	wchar_t				test[50];



	system("wmic bios get serialnumber > BIOS.txt");

	FILE* fp = fopen("BIOS.txt", "r, ccs=UTF-8");

	fgetws(test, 30, fp);

	while (!feof(fp))				/*	read from the file as wchar arrays	*/
	{
		wchar_t		serial[50];
		fgetws(serial, 50, fp);

		serialNumberW.push_back(serial);
	}

	fclose(fp);
	remove("BIOS.txt");


	string	defaultString = "Default string";


	for (int i = 0; i < serialNumberW.size(); ++i)
	{
		string		seriall = Utils::ws2s(serialNumberW[i]);			/*	format the strings	*/
		seriall = Utils::rtrim(seriall);


		if (seriall.compare(defaultString) != 0)
		{
			if (seriall.length() > 6)
			{
				serialNumbers.push_back(seriall);
			}
		}
	}

	return serialNumbers;
}







void	spoofBIOS()
{
	INPUT_STRUCT	input;


	vector<string>		serialNumbers = getBIOSserials();


	for (int i = 0; i < serialNumbers.size(); ++i)
	{
		cout << serialNumbers[i] << endl;

		int		length = serialNumbers[i].size();

		serialNumbers[i] += Globals::signatureGuard;

		strcpy((char*)input.serialNumber, serialNumbers[i].c_str());


		input.serialLength = length;

		DWORD	bytesReturn;


		BOOL	status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
			sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);



		if (status == FALSE)
		{
			cout << "[+] there was an error!  getlasterror " << GetLastError() << endl;
		}
	}


	return;
}

void	spoofRAM()
{
	INPUT_STRUCT	input;


	vector<string>		serialNumbers = getRAMserials();


	for (int i = 0; i < serialNumbers.size(); ++i)
	{
		cout << serialNumbers[i] << endl;

		int		length = serialNumbers[i].size();

		serialNumbers[i] += Globals::signatureGuard;

		strcpy((char*)input.serialNumber, serialNumbers[i].c_str());


		input.serialLength = length;
		input.wide = false;


		DWORD	bytesReturn;


		BOOL	status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
			sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);



		if (status == FALSE)
		{
			cout << "[+] there was an error!  getlasterror " << GetLastError() << endl;
		}
	}


	return;
}
#pragma once
#include "utils.h"


string randomGPU()
{

	string    String = "GPU-";

	for (int i = 0; i < 36; ++i)
	{

		if ((i != 8) && (i != 13) && (i != 18) && (i != 23))
		{
			int seed = rand();

			String += (GPUchars[seed % (sizeof(GPUchars) - 1)]);
		}
		else
		{
			String += "-";
		}
	}

	return String;
}




vector<string>	getGPUuuid()
{

	vector<string>		serialNumbers;

	vector<wstring>		serialNumberW;

	wchar_t				test[50];



	system("nvidia-smi --query-gpu=gpu_uuid --format=csv > GPUuuid.txt");

	FILE* fp = fopen("GPUuuid.txt", "r, ccs=UTF-8");

	fgetws(test, 30, fp);

	while (!feof(fp))				/*	read from the file as wchar arrays	*/
	{
		wchar_t		serial[50];
		fgetws(serial, 50, fp);

		serialNumberW.push_back(serial);
	}

	fclose(fp);
	remove("GPUuuid.txt");


	serialNumberW.pop_back();


	for (int i = 0; i < serialNumberW.size(); ++i)
	{
		string		seriall = Utils::ws2s(serialNumberW[i]);			/*	format the strings	*/
		seriall = Utils::rtrim(seriall);

		seriall = seriall.substr(0, Globals::sizeLimit);

		serialNumbers.push_back(seriall);
	}

	return serialNumbers;
}







void	spoofGPU()
{
	INPUT_STRUCT		input;

	vector<string>		serialNumbers = getGPUuuid();

	DWORD				bytesReturn;


	for (int i = 0; i < serialNumbers.size(); ++i)
	{
		cout << serialNumbers[i] << endl;


		string		spoofString = randomGPU();

		int			length = 16;


		input.serialLength = length;

		serialNumbers[i] += Globals::signatureGuard;

		strcpy((char*)input.serialNumber, serialNumbers[i].c_str());

		input.wide = false;

		


		BOOL	status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
			sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);



		if (status == FALSE)
		{
			cout << "[+] there was an error!  getlasterror " << GetLastError() << endl;
		}
	}


	return;
}
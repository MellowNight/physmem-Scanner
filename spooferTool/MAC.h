#pragma once
#include "utils.h"

struct NIC_MAC
{
	BYTE   a[6];
};







vector<NIC_MAC>		getNicMac()
{

	wchar_t				test[50];





	system("getmac > mac.txt");

	FILE* fp = fopen("mac.txt", "r, ccs=UTF-8");


	fgetws(test, 30, fp);
	fgetws(test, 30, fp);


	while (!feof(fp))				/*	read from the file as wchar arrays	*/
	{
		NIC_MAC		serial[50];
		fgetws(serial, 50, fp);

		serialNumberW.push_back(serial);
	}

	fclose(fp);
	remove("GPUuuid.txt");


}
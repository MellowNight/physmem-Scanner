#pragma once
#include    <Windows.h>
#include    <iostream>
#include	<fstream>
#include	<string>
#include	<wchar.h>
#include	<locale>
#include	<codecvt>
#include	<vector>
#include	<algorithm> 
#include	<functional> 
#include	<cctype>
#include	<locale>



#define    SCAN_PHYSICAL_MEMORY  	CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)

using namespace std;

struct INPUT_STRUCT
{
    BYTE        serialNumber[50];
    int         serialLength;
    bool        wide;
};

namespace Globals
{
    char        signatureGuard[] = "pvgq";
    HANDLE		driverHandle;
}
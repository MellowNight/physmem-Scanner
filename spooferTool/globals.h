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
#include    <thread>
#include    <filesystem>

#define    SCAN_PHYSICAL_MEMORY  	CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define    SPOOF_SMBIOS           	CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)

using namespace std;

struct  INPUT_STRUCT
{
    BYTE        serialNumber[60];
    int         serialLength;
    bool        wide;
    BYTE        spoofString[60];
};

namespace Globals
{
    char        signatureGuard[] = "pvgq";
    HANDLE		driverHandle;
    int         sizeLimit = 14;
}



const char alphanum[] ="01289OPQRSTUVWXYZabcdefghijklmnABCDEFGHIJKLMNopgrstu34567vwxyz";

const char DiskChars[] = "345QRSTUV6789ABCDEFGHIJKLMNOP012WXYZ";

const char GPUchars[] = "06789abcd12345ef";
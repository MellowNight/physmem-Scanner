#pragma once
#include	 "utils.h"
#include	  <iphlpapi.h>
#include      <ntddndis.h>

#pragma comment(lib, "iphlpapi.lib")



/*          
    PROBLEM:

    scanning for mac serial and replacing will make internet disconnect.


*/











struct NIC_INFO
{
    int         macLength;
	BYTE        mac[12];       /*   extra space for signature guard     */
    char        name[60];
};


string randomMac(int length)
{
    Sleep(1000);


    time_t seconds;

    seconds = time(NULL);

    srand(seconds);

    string    String;

    for (int i = 0; i < length; ++i)
    {
        int seed = rand();

        String += (DiskChars[seed % (sizeof(DiskChars) - 1)]);
    }

    return String;
}



void    printMacAddr(PIP_ADAPTER_INFO   pAdapter)
{
    printf("\tAdapter Addr: \t");

    for (UINT i = 0; i < pAdapter->AddressLength; i++)
    {

        if (i == (pAdapter->AddressLength - 1))
        {
            printf("-%02X\n", (int)pAdapter->Address[i]);
        }
        else
        {
            printf("%02X-", (int)pAdapter->Address[i]);
        }
    }
}


void    printMacAddr(BYTE* mac)
{
    printf("\tAdapter Addr: \t");

    int macLength = 6;

    for (UINT i = 0; i < macLength; i++)
    {

        if (i == (macLength - 1))
        {
            printf("%02X\n", (int)mac[i]);
        }
        else
        {
            printf("%02X-", (int)mac[i]);
        }
    }
}


vector<NIC_INFO>		getNicMac()
{

    vector<NIC_INFO>    nicInfoArray;


    PIP_ADAPTER_INFO pAdapterInfo;

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));

    ULONG buflen = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo(pAdapterInfo, &buflen) == ERROR_BUFFER_OVERFLOW) {

        free(pAdapterInfo);

        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(buflen);
    }

    if (GetAdaptersInfo(pAdapterInfo, &buflen) == NO_ERROR) {

        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;

        while (pAdapter) {

            NIC_INFO    nicInfo;


            printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);

            printMacAddr(pAdapter);

            RtlCopyMemory(nicInfo.mac, pAdapter->Address, pAdapter->AddressLength);
            RtlCopyMemory(nicInfo.mac + 6, Globals::signatureGuard, 5);
            RtlCopyMemory(nicInfo.name, pAdapter->AdapterName, 60);
            nicInfo.macLength = pAdapter->AddressLength;



            nicInfoArray.push_back(nicInfo);


            pAdapter = pAdapter->Next;
        }
    }


    return nicInfoArray;

}




unsigned int code = OID_802_3_CURRENT_ADDRESS;

int   spoofMac()
{
    vector<NIC_INFO>    nicInfoArray;


    nicInfoArray = getNicMac();


    DWORD bytesReturned;



    for (int i = 0; i < nicInfoArray.size(); ++i)
    {
        INPUT_STRUCT input;

        input.serialLength = nicInfoArray[i].macLength;
        input.wide = false;
        RtlCopyMemory(input.spoofString, randomMac(6).c_str(), 6);
        RtlCopyMemory(input.serialNumber, nicInfoArray[i].mac, 8);

        

        BOOL	status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
            sizeof(INPUT_STRUCT), 0, 0, &bytesReturned, 0);


        if (status == FALSE)
        {
            cout << "[+] there was an error!  getlasterror " << GetLastError() << endl;

            if (GetLastError() == 6)
            {
                cout << "you need to run as admin! " << endl;
            }
        }
    }

    return 0;
}
#pragma once
#include "Undocumented.h"


struct INPUT_STRUCT
{
    BYTE        serialNumber[60];
    int         serialLength;
    bool        wide;
    BYTE        spoofString[60];
};



struct PAGE_HELPER
{
    PVOID       reservedPage;
    PTE_64*     reservedPagePTE;    
};


namespace Globals
{
    bool              killThread = false;
    char              spoofString[80];
    char              signatureGuard[] = "pvgq";
    PAGE_HELPER       reservedPages[5];
    int               sizeLimit = 14;
    PEPROCESS         targetProcess;
    HANDLE             processID;
};






namespace Utils
{
    HANDLE    GetProcessPID(PCWSTR     processName) 
    {

        NTSTATUS status = STATUS_SUCCESS;
        PVOID buffer;


        buffer = ExAllocatePoolWithTag(NonPagedPool, 1024 * 1024, 'enoN');

        if (!buffer) {
            DbgPrint("couldn't allocate memory \n");
            return 0;
        }

        DbgPrintEx(0, 0, "Process list allocated at address %#x\n", buffer);

        PSYSTEM_PROCESS_INFORMATION pInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;


        status = ZwQuerySystemInformation(SystemProcessInformation, pInfo, 1024 * 1024, NULL);
        if (!NT_SUCCESS(status)) {
            DbgPrintEx(0, 0, "ZwQuerySystemInformation Failed : STATUS CODE : %p\n", status);
            ExFreePoolWithTag(buffer, 'Enon');
            return 0;
        }

        UNICODE_STRING WantedImageName;

        RtlInitUnicodeString(&WantedImageName, processName);

        if (NT_SUCCESS(status)) {
            for (;;) {
                DbgPrintEx(0, 0, "\nProcess name: %ws | Process ID: %d\n", pInfo->ImageName.Buffer, pInfo->ProcessId); // Display process information.
                if (RtlEqualUnicodeString(&pInfo->ImageName, &WantedImageName, TRUE)) {
                    return pInfo->ProcessId;
                    break;
                }
                else if (pInfo->NextEntryOffset)
                    pInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)pInfo + pInfo->NextEntryOffset);
                else
                    break;
            }
        }

        ExFreePoolWithTag(buffer, 'enoN');
    }


    void wprint(wchar_t* buffer, int wchars)
    {
        for (int i = 0; i < wchars; ++i)
        {
            DbgPrint("%wc", buffer[i]);
        }
        DbgPrint("\n");
    }

    void print(char*  buffer, int chars)
    {
        for (int i = 0; i < chars; ++i)
        {
            DbgPrint("%c", buffer[i]);
        }
        DbgPrint("\n");
    }

    PVOID64     findPattern(BYTE* pattern, int patternSize, BYTE    wildCard, ULONG64 startAddress, ULONG64   endAddress)
    {
        bool found = false;


        for (BYTE* i = (BYTE*)startAddress; i < (BYTE*)(endAddress - patternSize); ++i)
        {
            found = true;

            for (int j = 0; j < patternSize; ++j)
            {
                if ((pattern[j] != i[j]) && (pattern[j] != wildCard))
                {
                    found = false;
                    break;
                }
            }
            if (found == true)
            {
                return (PVOID64)i;
            }
        }

        return 0;
    }




}

extern "C" { 
    NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
}




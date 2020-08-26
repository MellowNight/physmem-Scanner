#pragma once
#include    <ntdef.h>
#include    <ntifs.h>
#include    <windef.h>
#include    <Ntstrsafe.h>
#include    <intrin.h>
#include    "ia32.h"


struct PAGE_HELPER
{
    PVOID       reservedPage;
    PTE_64*     reservedPagePTE;    
};


namespace Globals
{
    bool              killThread = false;
    char              spoofString[] = "SPOOFED-BY-JGUO5258";
    char              signatureGuard[] = "pvgq";
    PAGE_HELPER       reservedPages[5];
};



struct SCAN_CONTEXT
{
    BYTE*       pattern;
    int         pml4Low;
    int         pml4High;
    int         length;
    int         threadNumber;
    PETHREAD    threadObject;
    HANDLE      threadHandle;
};


namespace Utils
{

    PVOID64     findPattern(BYTE* pattern, int patternSize, BYTE    wildCard, ULONG64 startAddress, ULONG64   endAddress)
    {
        bool found = false;

        for (BYTE* i = (BYTE*)startAddress; i < (BYTE*)(endAddress - patternSize); ++i)
        {

            found = true;

            for (int j = 0; j < patternSize; ++j)
            {
                if ((ULONG64)(i + j) > endAddress)
                {
                    if (!MmIsAddressValid(i + j))
                    {
                        return 0;
                    }
                }

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




#pragma once
#include "pageManager.h"






namespace Globals
{
    bool              killThread = false;
    char              spoofString[80];
    char              signatureGuard[] = "pvgq";
    int               sizeLimit = 14;
    PEPROCESS         targetProcess;
    HANDLE            processID;
    PAGE_MANAGER      pageManager;
};




struct INPUT_STRUCT
{
    BYTE        serialNumber[60];
    int         serialLength;
    bool        wide;
    BYTE        spoofString[60];
};







namespace Utils
{
    PVOID getDriverBaseAddress(OUT PULONG pSize, const char* driverName)
    {
        NTSTATUS Status = STATUS_SUCCESS;
        ULONG Bytes = 0;
        PRTL_PROCESS_MODULES arrayOfModules;


        PVOID			DriverBase = 0;
        ULONG64			DriverSize = 0;


        //get size of system module information
        Status = ZwQuerySystemInformation(SystemModuleInformation, 0, Bytes, &Bytes);
        if (Bytes == 0)
        {
            DbgPrint("%s: Invalid SystemModuleInformation size\n");
            return NULL;
        }


        arrayOfModules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, Bytes, 0x45454545); //array of loaded kernel modules
        RtlZeroMemory(arrayOfModules, Bytes); //clean memory


        Status = ZwQuerySystemInformation(SystemModuleInformation, arrayOfModules, Bytes, &Bytes);

        if (NT_SUCCESS(Status))
        {
            PRTL_PROCESS_MODULE_INFORMATION pMod = arrayOfModules->Modules;
            for (int i = 0; i < arrayOfModules->NumberOfModules; ++i)
            {
                //list the module names:

                DbgPrint("Image name: %s\n", pMod[i].FullPathName + pMod[i].OffsetToFileName);
                // path name plus some amount of characters will lead to the name itself
                const char* DriverName = (const char*)pMod[i].FullPathName + pMod[i].OffsetToFileName;

                if (strcmp(DriverName, driverName) == 0)
                {
                    DbgPrint("found driver\n");


                    DriverBase = pMod[i].ImageBase;
                    DriverSize = pMod[i].ImageSize;

                    DbgPrint("kernel module Size : %i\n", DriverSize);
                    DbgPrint("kernel module Base : %p\n", DriverBase);


                    if (arrayOfModules)
                        ExFreePoolWithTag(arrayOfModules, 0x45454545); // 'ENON'


                    if (pSize != NULL)
                    {
                        *pSize = DriverSize;
                    }

                    return DriverBase;
                }
            }
        }
        if (arrayOfModules)
            ExFreePoolWithTag(arrayOfModules, 0x45454545); // 'ENON'


        if (pSize != NULL)
        {
            *pSize = DriverSize;
        }
        return (PVOID)DriverBase;
    }




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

        if (!MmIsAddressValid((PVOID)startAddress))
        {
            return 0;
        }

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


    PVOID ResolveRelativeAddress(_In_ PVOID Instruction, _In_ ULONG OffsetOffset, _In_ ULONG InstructionSize)
    {
        ULONG_PTR Instr = (ULONG_PTR)Instruction;
        LONG RipOffset = *(PLONG)(Instr + OffsetOffset);
        PVOID ResolvedAddr = (PVOID)(Instr + InstructionSize + RipOffset);

        return ResolvedAddr;
    }


}





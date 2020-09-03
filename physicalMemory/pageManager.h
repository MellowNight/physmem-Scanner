#pragma once
#include "Undocumented.h"


namespace Memory
{
    PT_ENTRY_64*    GetPte(VOID* VirtualAddress, CR3 HostCr3);


}


struct PAGE_HELPER
{
    PVOID       reservedPage;
    PTE_64*     reservedPagePTE;
};





class   PAGE_MANAGER
{

public:


    PAGE_HELPER       reservedPages[5];




    void    initializePages()
    {
        CR3     cr3;

        cr3.Flags = __readcr3();

        for (int i = 0; i < 5; ++i)
        {
            reservedPages[i].reservedPage = MmAllocateMappingAddress(PAGE_SIZE, 12819636 + i);

            reservedPages[i].reservedPagePTE = (PTE_64*)Memory::GetPte(reservedPages[i].reservedPage, cr3);
        }
    }




    void    destroyPages()
    {
        for (int i = 0; i < 5; ++i)
        {
            MmFreeMappingAddress(reservedPages[i].reservedPage, 12819636 + i);
        }
    }





    PVOID    mapPage(PVOID   physicalAddr, int id)
    {
        reservedPages[id].reservedPagePTE->Write = true;
        reservedPages[id].reservedPagePTE->Present = true;

        reservedPages[id].reservedPagePTE->PageFrameNumber = (DWORD64)physicalAddr >> PAGE_SHIFT;


        /*  cause an exception to flush TLB     credits: xerox      */

        //MmCopyVirtualMemory(PsGetCurrentProcess(), &myInt, Globals::targetProcess, 0, sizeof(int), KernelMode, &outSize);

        __invlpg(reservedPages[id].reservedPage);

        return (PVOID)((DWORD64)(reservedPages[id].reservedPage) + ((DWORD64)physicalAddr & PAGE_MASK));
    }




    PVOID    UnmapPage(PVOID   physicalAddr, int id)
    {
        reservedPages[id].reservedPagePTE->Flags = 0;

        /*  cause an exception to flush TLB     credits: xerox      */

        //MmCopyVirtualMemory(PsGetCurrentProcess(), &myInt, Globals::targetProcess, 0, sizeof(int), KernelMode, &outSize);

        __invlpg(reservedPages[id].reservedPage);

        return 0;
    }


};





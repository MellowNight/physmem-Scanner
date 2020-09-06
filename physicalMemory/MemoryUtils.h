#pragma once
#include "Utils.h"


enum mapLevel
{
    PML4_LEVEL,
    PDPT_LEVEL,
    PD_LEVEL,
    PT_LEVEL,
    PTE_LEVEL,
};




namespace  Memory
{
    int       myInt = 999999;
    int       myInt2 = 1312312;
    SIZE_T    outSize;
    PHYSICAL_MEMORY_RANGE   physicalMemRange[10];
    static  int             numberOfRuns = 0;



    PT_ENTRY_64*     GetPte(VOID* VirtualAddress, CR3 HostCr3)
    {
        ADDRESS_TRANSLATION_HELPER helper;
        UINT32 level;
        PT_ENTRY_64* finalEntry;
        PML4E_64* pml4;
        PML4E_64* pml4e;
        PDPTE_64* pdpt;
        PDPTE_64* pdpte;
        PDE_64* pd;
        PDE_64* pde;
        PTE_64* pt;
        PTE_64* pte;

        helper.AsUInt64 = (UINT64)VirtualAddress;

        PHYSICAL_ADDRESS    addr;



        addr.QuadPart = HostCr3.AddressOfPageDirectory << PAGE_SHIFT;

        pml4 = (PML4E_64*)MmGetVirtualForPhysical(addr);

        pml4e = &pml4[helper.AsIndex.Pml4];

        if (pml4e->Present == FALSE)
        {
            finalEntry = (PT_ENTRY_64*)pml4e;
            goto Exit;
        }




        addr.QuadPart = pml4e->PageFrameNumber << PAGE_SHIFT;

        pdpt = (PDPTE_64*)MmGetVirtualForPhysical(addr);

        pdpte = &pdpt[helper.AsIndex.Pdpt];

        if ((pdpte->Present == FALSE) || (pdpte->LargePage != FALSE))
        {
            finalEntry = (PT_ENTRY_64*)pdpte;
            goto Exit;
        }



        addr.QuadPart = pdpte->PageFrameNumber << PAGE_SHIFT;

        pd = (PDE_64*)MmGetVirtualForPhysical(addr);

        pde = &pd[helper.AsIndex.Pd];

        if ((pde->Present == FALSE) || (pde->LargePage != FALSE))
        {
            finalEntry = (PT_ENTRY_64*)pde;
            goto Exit;
        }





        addr.QuadPart = pde->PageFrameNumber << PAGE_SHIFT;

        pt = (PTE_64*)MmGetVirtualForPhysical(addr);

        pte = &pt[helper.AsIndex.Pt];

        finalEntry = (PT_ENTRY_64*)pte;

        return  (PT_ENTRY_64*)pte;

    Exit:
        return finalEntry;
    }





    void scanPage(INPUT_STRUCT* context, PVOID64    virtualPage, ULONG     size, PHYSICAL_ADDRESS  addr)
    {

        if (virtualPage && MmIsAddressValid(virtualPage))
        {


            //MmCopyVirtualMemory(PsGetCurrentProcess(), &myInt, Globals::targetProcess, 0, sizeof(int), KernelMode, &outSize);
            __invlpg(virtualPage);


            //DbgPrint("[+] valid page at physical address %p ! scanning...\n", addr.QuadPart);

            /*  choose any wildcard, it may improve performance and accuracy    */

            PVOID64     foundAt = Utils::findPattern(context->serialNumber, context->serialLength, 'A',
                (ULONG64)virtualPage, (ULONG64)virtualPage + size);

            if (foundAt)
            {
                DbgPrint("[+] serial number found at physical address %p !!\n",
                    (addr.QuadPart + ((DWORD64)foundAt - (DWORD64)virtualPage)));

                if (context->wide == false)
                {
                    Utils::print((char*)foundAt, 18);
                }
                else
                {
                    Utils::wprint((wchar_t*)foundAt, 18);
                }

                if (memcmp(Globals::signatureGuard, (PVOID)((DWORD64)(foundAt)+context->serialLength), 3) == 0)
                {
                    DbgPrint("this is our own pattern, lets not spoof this one \n");
                }
                else
                {
                    DbgPrint("spoofing... \n");
                    RtlCopyMemory(foundAt, Globals::spoofString, context->serialLength);
                }
            }
            else
            {
            }

        }
    }




    void    getPhysicalMemoryRanges()
    {
        PPHYSICAL_MEMORY_RANGE MmPhysicalMemoryRange = MmGetPhysicalMemoryRanges();

        numberOfRuns = 0;

        for (int number_of_runs = 0;
            (MmPhysicalMemoryRange[number_of_runs].BaseAddress.QuadPart) || (MmPhysicalMemoryRange[number_of_runs].NumberOfBytes.QuadPart);
            number_of_runs++)
        {

            DbgPrint("base addr %llx, size %llx\n", MmPhysicalMemoryRange[number_of_runs].BaseAddress.QuadPart,
                MmPhysicalMemoryRange[number_of_runs].NumberOfBytes.QuadPart);


            physicalMemRange[number_of_runs] = MmPhysicalMemoryRange[number_of_runs];

            numberOfRuns += 1;


        }

        return;
    }


    bool    isAddressinPhysMemRange(PHYSICAL_ADDRESS  addr)
    {
        for (int i = 0; i < numberOfRuns; ++i)
        {
            if ((addr.QuadPart >= physicalMemRange[i].BaseAddress.QuadPart) 
                && (addr.QuadPart <= (physicalMemRange[i].BaseAddress.QuadPart + physicalMemRange[i].NumberOfBytes.QuadPart)))
            {
                return true;
            }
        }

        return false;
    }



    void	scanPhysicalMemory(_In_ PVOID Context)
    {
        
        DbgPrint("thread start! \n");

        getPhysicalMemoryRanges();

        INPUT_STRUCT* context = (INPUT_STRUCT*)Context;



        CR3     cr3;
        cr3.Flags = __readcr3();


        PHYSICAL_ADDRESS   addr;

        addr.QuadPart = cr3.AddressOfPageDirectory << PAGE_SHIFT;


        DbgPrint("base address of page directory is %p \n", addr.QuadPart);


        PML4E_64* pml4 = (PML4E_64*)Globals::pageManager.mapPage((PVOID)addr.QuadPart, PML4_LEVEL);




        if (MmIsAddressValid(pml4) == FALSE || pml4 == 0)
        {
            DbgPrint("error! pml4 not valid address \n");
            return;
        }


        /*  PML4 -  use reserved page 1      */

        for (int i = 0; i < 512; ++i)
        {

            if (pml4[i].Present == false)
            {
                continue;
            }

            addr.QuadPart = pml4[i].PageFrameNumber << PAGE_SHIFT;

            PDPTE_64* pdpt = (PDPTE_64*)Globals::pageManager.mapPage((PVOID)addr.QuadPart, PDPT_LEVEL);


            if (pdpt == 0 || (MmIsAddressValid(pdpt) == FALSE))
            {
                continue;
            }

            /*  PDPT -  use reserved page 2    */

            for (int ii = 0; ii < 512; ++ii)
            {

                addr.QuadPart = pdpt[ii].PageFrameNumber << PAGE_SHIFT;


                if (pdpt[ii].Present == false)
                {
                    continue;
                }
                if (pdpt[ii].LargePage == true)
                {

                    DbgPrint("[+] this is 1 gigabyte large page for pdpt \n");

                    PDE_64* pageDir = (PDE_64*)MmGetVirtualForPhysical(addr);


                    if (pageDir == 0 || (MmIsAddressValid(pageDir) == FALSE))
                    {
                        continue;
                    }

                    for (int kkk = 0; kkk < 262144; ++kkk)
                    {

                        scanPage(context, (pageDir + (kkk * PAGE_SIZE)), PAGE_SIZE, addr);

                    }

                    continue;
                }


                PDE_64* pageDir = (PDE_64*)Globals::pageManager.mapPage((PVOID)addr.QuadPart, PD_LEVEL);


                if (pageDir == 0 || (MmIsAddressValid(pageDir) == FALSE))
                {
                    continue;
                }


                /*  PD (page directory) -  use reserved page 3    */

                for (int iii = 0; iii < 512; ++iii)
                {

                    addr.QuadPart = pageDir[iii].PageFrameNumber << PAGE_SHIFT;


                    if (pageDir[iii].Present == false)
                    {
                        continue;
                    }
                    if (pageDir[iii].LargePage == true)
                    {
                        DbgPrint("[+] this is 2 megabyte large page for PDE\n");


                        PTE_64* pageTable = (PTE_64*)MmGetVirtualForPhysical(addr);


                        scanPage(context, pageTable, PAGE_SIZE * 512, addr);


                        continue;
                    }



                    PTE_64* pageTable = (PTE_64*)Globals::pageManager.mapPage((PVOID)addr.QuadPart, PT_LEVEL);


                    if (pageTable == 0 || (MmIsAddressValid(pageTable) == FALSE))
                    {
                        continue;
                    }




                    /*  PT (page table) -  use reserved page 4    */

                    for (int iiii = 0; iiii < 512; ++iiii)
                    {
                        if (pageTable[iiii].Present == false || pageTable[iiii].Write == false)
                        {
                            continue;
                        }

                        addr.QuadPart = pageTable[iiii].PageFrameNumber << PAGE_SHIFT;



                        if (isAddressinPhysMemRange(addr) == TRUE)
                        {
                    
                            PVOID64  virtualPage = (PVOID64)MmMapIoSpace(addr, PAGE_SIZE, MmNonCached);

                            /*    PTE - use reserved page 5      */

                            scanPage(context, virtualPage, PAGE_SIZE, addr);

                            MmUnmapIoSpace(virtualPage, PAGE_SIZE);
                        }
                    }

                    Globals::pageManager.UnmapPage((PVOID)addr.QuadPart, 3);
                }

                Globals::pageManager.UnmapPage((PVOID)addr.QuadPart, 2);
            }

            Globals::pageManager.UnmapPage((PVOID)addr.QuadPart, 1);
        }

        DbgPrint("done!  \n\n\n\n\n\n\n");

        Globals::pageManager.UnmapPage((PVOID)addr.QuadPart, 0);

        return;
    
    }
}

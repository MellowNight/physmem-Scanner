#pragma once
#include "Utils.h"



namespace  Memory
{

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

        DbgPrint("base of PML4 directory physical address is: %p \n", addr.QuadPart);

        pml4 = (PML4E_64*)MmGetVirtualForPhysical(addr);

        pml4e = &pml4[helper.AsIndex.Pml4];

        if (pml4e->Present == FALSE)
        {
            finalEntry = (PT_ENTRY_64*)pml4e;
            goto Exit;
        }




        addr.QuadPart = pml4e->PageFrameNumber << PAGE_SHIFT;

        DbgPrint("PML4E physical address is: %p \n", addr.QuadPart);

        pdpt = (PDPTE_64*)MmGetVirtualForPhysical(addr);

        pdpte = &pdpt[helper.AsIndex.Pdpt];

        if ((pdpte->Present == FALSE) || (pdpte->LargePage != FALSE))
        {
            finalEntry = (PT_ENTRY_64*)pdpte;
            goto Exit;
        }



        addr.QuadPart = pdpte->PageFrameNumber << PAGE_SHIFT;

        DbgPrint("PDPTE physical address is: %p \n", addr.QuadPart);

        pd = (PDE_64*)MmGetVirtualForPhysical(addr);

        pde = &pd[helper.AsIndex.Pd];

        if ((pde->Present == FALSE) || (pde->LargePage != FALSE))
        {
            finalEntry = (PT_ENTRY_64*)pde;
            goto Exit;
        }





        addr.QuadPart = pde->PageFrameNumber << PAGE_SHIFT;

        DbgPrint("PD physical address is: %p \n", addr.QuadPart);

        pt = (PTE_64*)MmGetVirtualForPhysical(addr);

        pte = &pt[helper.AsIndex.Pt];

        finalEntry = (PT_ENTRY_64*)pte;


        addr.QuadPart = pte->PageFrameNumber << PAGE_SHIFT;

        DbgPrint("PTE physical address is: %p \n", addr.QuadPart);

        return  (PT_ENTRY_64*)MmGetVirtualForPhysical(addr);

    Exit:
        return finalEntry;
    }



    void scanPage(SCAN_CONTEXT* context, PVOID64    virtualPage, ULONG     size, PHYSICAL_ADDRESS  addr, BOOLEAN    MapIoSpace)
    {
        if (virtualPage && MmIsAddressValid(virtualPage))
        {
            //  DbgPrint("[+] valid page at physical address %p ! scanning...\n", addr.QuadPart);

            __invlpg(virtualPage);

            /*  choose any wildcard, it may improve performance and accuracy    */

            PVOID64     foundAt = Utils::findPattern(context->pattern, context->length, 'a',
                (ULONG64)virtualPage, (ULONG64)virtualPage + size);

            if (foundAt)
            {
                DbgPrint("[+] serial number found at physical address %p !!\n",
                    (addr.QuadPart + ((DWORD64)foundAt - (DWORD64)virtualPage)));
                DbgPrint(" %s \n", foundAt);

                if (memcmp(Globals::signatureGuard, (PVOID)((DWORD64)(foundAt)+context->length), 3) == 0)
                {
                    DbgPrint("this is our own pattern, lets not spoof this one \n");
                }
                else
                {
                    DbgPrint("spoofing... \n");
                    RtlCopyMemory(foundAt, Globals::spoofString, 20);
                }
            }
            else
            {
            }

        }
        else if (MapIoSpace == TRUE)
        {
            /*  one page at a time    (for large pages)  */

            PHYSICAL_ADDRESS    temp;

            for (int jj = 0; jj < ((size) / PAGE_SIZE); ++jj)
            {

                temp.QuadPart = (jj * PAGE_SIZE);

                addr = temp;

                virtualPage = MmMapIoSpace(addr, PAGE_SIZE, MmNonCached);

                if (virtualPage && MmIsAddressValid(virtualPage))
                {
                    //  DbgPrint("[+] valid page at physical address %p ! scanning...\n", addr.QuadPart);

                    //__invlpg(virtualPage);

                    /*  choose any wildcard, it may improve performance and accuracy    */

                    PVOID64     foundAt = Utils::findPattern(context->pattern,
                        context->length, 'a', (ULONG64)virtualPage, PAGE_SIZE);

                    if (foundAt)
                    {
                        DbgPrint("[+] serial number found at physical address %p !!\n",
                            (addr.QuadPart + ((DWORD64)foundAt - (DWORD64)virtualPage)));
                        DbgPrint(" %s \n", foundAt);

                        if (RtlCompareMemory((PVOID64)((DWORD64)foundAt + context->length), Globals::signatureGuard, 3) > 2)
                        {
                            DbgPrint("this is our own pattern, lets not spoof this one \n");
                        }
                        else
                        {
                            DbgPrint("spoofing... \n");
                            RtlCopyMemory(foundAt, Globals::spoofString, 20);
                        }
                    }
                    else
                    {
                    }
                }
                else
                {
                    DbgPrint("MmMapIoSpace problem! \n");
                    MmUnmapIoSpace(virtualPage, size);

                }

                if (virtualPage)
                {
                    MmUnmapIoSpace(virtualPage, size);
                }
            }

        }
        else
        {


        }
    }



    void    initializePages()
    {
        CR3     cr3;

        cr3.Flags = __readcr3();

        for (int i = 0; i < 5; ++i)
        {
            Globals::reservedPages[i].reservedPage = ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, 12389680 + i);

            Globals::reservedPages[i].reservedPagePTE = (PTE_64*)GetPte(Globals::reservedPages[i].reservedPage, cr3);

            DbgPrint("globals::reservedpages no. %i address %p \n", i, Globals::reservedPages[i].reservedPage);
            DbgPrint("globals::reservedpages no. %i PTE address %p \n", i, Globals::reservedPages[i].reservedPagePTE);
        }
    }

    void    destroyPages()
    {
        for (int i = 0; i < 5; ++i)
        {
            ExFreePoolWithTag(Globals::reservedPages[i].reservedPage, 12389680 + i);
        }
    }


    PVOID    mapPhysicalAddr(PVOID   physicalAddr, int pageLevel)
    {
        Globals::reservedPages[pageLevel].reservedPagePTE->Write = true;

        Globals::reservedPages[pageLevel].reservedPagePTE->Present = true;

        Globals::reservedPages[pageLevel].reservedPagePTE->PageFrameNumber = (DWORD64)physicalAddr >> PAGE_SHIFT;


        __invlpg(Globals::reservedPages[pageLevel].reservedPage);

        return (PVOID)((DWORD64)(Globals::reservedPages[pageLevel].reservedPage) + ((DWORD64)physicalAddr & PAGE_MASK));
    }

    PVOID    UnmapPhysicalAddr(PVOID   physicalAddr, int pageLevel)
    {
        Globals::reservedPages[pageLevel].reservedPagePTE->Flags = 0;
        __invlpg(Globals::reservedPages[pageLevel].reservedPage);

        return 0;
    }


    void	scanPhysicalMemory(_In_ PVOID Context)
    {
        DbgPrint("thread start! \n");

        SCAN_CONTEXT* context = (SCAN_CONTEXT*)Context;


        CR3     cr3;
        cr3.Flags = __readcr3();

        PHYSICAL_ADDRESS   addr;
        addr.QuadPart = cr3.AddressOfPageDirectory << PAGE_SHIFT;

        DbgPrint("base address of page directory is %p \n", addr.QuadPart);


        PML4E_64* pml4 = (PML4E_64*)mapPhysicalAddr((PVOID)addr.QuadPart, 0);



        DbgPrint("virtual base address of page directory 1 is %p \n", MmGetVirtualForPhysical(addr));
        DbgPrint("virtual base address of page directory 2 is %p \n", pml4);


        if (MmIsAddressValid(pml4) == FALSE || pml4 == 0)
        {
            DbgPrint("error! pml4 not valid address \n");
            PsTerminateSystemThread(STATUS_SUCCESS);
        }


        /*  PML4 -  use reserved page 1      */

        for (int i = context->pml4Low; i < context->pml4High; ++i)
        {
            if (pml4[i].Present == false)
            {
                continue;
            }

            addr.QuadPart = pml4[i].PageFrameNumber << PAGE_SHIFT;

            PDPTE_64* pdpt = (PDPTE_64*)mapPhysicalAddr((PVOID)addr.QuadPart, 1);


            if (pdpt == 0 || (MmIsAddressValid(pdpt) == FALSE))
            {
                continue;
            }

            /*  PDPT -  use reserved page 2    */

            for (int ii = 0; ii < 512; ++ii)
            {

                if (pdpt[ii].Present == false)
                {
                    continue;
                }
                if (pdpt[ii].LargePage == true)
                {


                    DbgPrint("[+] this is 1 gigabyte large page for pdpt \n");


                    addr.QuadPart = pdpt[ii].PageFrameNumber << PAGE_SHIFT;


                    PTE_64* pageDir = (PTE_64*)MmGetVirtualForPhysical(addr);


                    if (pageDir == 0 || (MmIsAddressValid(pageDir) == FALSE))
                    {
                        continue;
                    }

                    scanPage(context, pageDir, PAGE_SIZE * 512 * 512, addr, TRUE);

                    continue;
                }


                addr.QuadPart = pdpt[ii].PageFrameNumber << PAGE_SHIFT;

                PDE_64* pageDir = (PDE_64*)mapPhysicalAddr((PVOID)addr.QuadPart, 2);


                if (pageDir == 0 || (MmIsAddressValid(pageDir) == FALSE))
                {
                    continue;
                }

                /*  PD (page directory) -  use reserved page 3    */

                for (int iii = 0; iii < 512; ++iii)
                {

                    if (pageDir[iii].Present == false)
                    {
                        continue;
                    }
                    if (pageDir[iii].LargePage == true)
                    {
                        DbgPrint("[+] this is 2 megabyte large page for PDE\n");

                        addr.QuadPart = pageDir[iii].PageFrameNumber << PAGE_SHIFT;


                        PTE_64* pageTable = (PTE_64*)MmGetVirtualForPhysical(addr);


                        if (pageTable == 0 || (MmIsAddressValid(pageTable) == FALSE))
                        {
                            continue;
                        }

                        scanPage(context, pageTable, 4096 * 512, addr, TRUE);

                        continue;
                    }

                    addr.QuadPart = pageDir[iii].PageFrameNumber << PAGE_SHIFT;

                    PTE_64* pageTable = (PTE_64*)mapPhysicalAddr((PVOID)addr.QuadPart, 3);


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


                        PVOID64  virtualPage = (PVOID64)mapPhysicalAddr((PVOID)addr.QuadPart, 4);


                        /*    PTE - use reserved page 5      */

                        scanPage(context, virtualPage, PAGE_SIZE, addr, TRUE);

                    }

                    //MmUnmapIoSpace(pageTable, PAGE_SIZE);
                }

                //MmUnmapIoSpace(pageDir, PAGE_SIZE);
            }

            //MmUnmapIoSpace(pdpt, PAGE_SIZE);
        }

        DbgPrint("thread number %i exit \n\n\n\n\n\n\n\n", context->threadNumber);

        return;
    }
}
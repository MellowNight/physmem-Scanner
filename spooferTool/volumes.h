#pragma once
#include	"utils.h"



static std::random_device              rd;
static std::mt19937                    gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);


std::string generate_uuid() {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}

int    uuidToByteArray(string uuid, static uint8_t* outbuf)
{
    // This algo will work for any size of uuid regardless where the hypen is placed
    // However, you have to make sure that the destination have enough space.

    int strCounter = 0;      // need two counters: one for uuid string (size=38) and
    static int hexCounter = 0;      // another one for destination adv_data (size=32)
    int i = 0;
    while (i < uuid.size())
    {
        if (uuid[strCounter] == '-')
        {
            strCounter++;
            continue;
        }

        // convert the character to string
        char str[2] = "\0";
        str[0] = uuid[strCounter];

        // convert string to int base 16
        outbuf[hexCounter] = (uint8_t)strtol(str, NULL, 16);

        i++;
        strCounter++;
        hexCounter += 1;
    }

    return hexCounter;
}





vector<string> 	getVolumeGuids()
{
    vector<string>     guids;

    BOOL    bFlag;
    TCHAR   Buf[MAX_PATH];           
    TCHAR   Drive[] = TEXT("c:\\"); 
    TCHAR   I;                      



    // Walk through legal drive letters, skipping floppies.
    for (I = TEXT('c'); I < TEXT('z'); I++)
    {
        
        Drive[0] = I;

        bFlag = GetVolumeNameForVolumeMountPoint(
            Drive,     
            Buf,       
            MAX_PATH); 


        if (bFlag)
        {
            guids.push_back(Utils::ws2s(wstring(Buf).substr(11, 25)));
        }
    }

    return guids;
}



void         printByteArray(char*   byteArray)
{
    for (int i = 0; i < 32; ++i)
    {
        int o = byteArray[i];

        cout << std::hex << o << " ";
    }

    cout << endl;
}



void	spoofVolumes()
{

    vector<string>    guids = getVolumeGuids();


    DWORD    bytesReturn = 0;

    
   for (int i = 0; i < guids.size(); ++i)
   {
        char     uuidBytes[32];

        INPUT_STRUCT    input;

        cout << "volume number " << i << " guid: " << guids[i] << endl;


        input.serialLength = guids[i].size();
        int     oldLength = guids[i].size();


        guids[i] += Globals::signatureGuard;
        input.wide = false;

        string spoofString = generate_uuid();

        strcpy((char*)input.spoofString, spoofString.c_str());
        strcpy((char*)input.serialNumber, guids[i].c_str());


//        BOOL	status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
//            sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);
  


        /*  phase 2: scan for byte array    */

        input.serialLength = uuidToByteArray(guids[i], (uint8_t*)uuidBytes);

        RtlCopyMemory(input.serialNumber, uuidBytes, input.serialLength);
        RtlCopyMemory((input.serialNumber + input.serialLength), Globals::signatureGuard, input.serialLength);
        RtlCopyMemory(input.spoofString, Utils::randomMac(input.serialLength).c_str(), input.serialLength);


        BOOL status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
            sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0);
        



        /*  phase 3: scan for wide UUIDs    


        input.serialLength = (oldLength * sizeof(wchar_t));
        input.wide = true;


        RtlCopyMemory((wchar_t*)input.serialNumber, Utils::s2ws(guids[i]).c_str(), input.serialLength);
        RtlCopyMemory((char*)(input.serialNumber + input.serialLength), Globals::signatureGuard, 5);
        RtlCopyMemory((wchar_t*)input.spoofString, Utils::s2ws(spoofString).c_str(), input.serialLength);



        //RtlCopyMemory(input.spoofString, spoofString.c_str(), length);


       status = DeviceIoControl(Globals::driverHandle, SCAN_PHYSICAL_MEMORY, &input,
            sizeof(INPUT_STRUCT), 0, 0, &bytesReturn, 0); */
    }
}



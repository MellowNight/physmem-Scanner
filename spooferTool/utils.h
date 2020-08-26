#pragma once
#include "globals.h"


namespace Utils
{

    VOID SwapEndianess(PCHAR dest, PCHAR src) {
        for (size_t i = 0, l = strlen(src); i < l; i += 2) {
            dest[i] = src[i + 1];
            dest[i + 1] = src[i];
        }
    }

    std::wstring s2ws(const std::string& str)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        return converterX.from_bytes(str);
    }

    std::string ws2s(const std::wstring& wstr)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        return converterX.to_bytes(wstr);
    }


    void removeSpaces(char* str)
    {
        {
            // To keep track of non-space character count 
            int count = 0;

            for (int i = 0; str[i]; i++)
                if (str[i] != ' ')
                    str[count++] = str[i]; // here count is 
                                           // incremented 
            str[count] = '\0';
        }

        return;
    }

    // trim from end
    static inline std::string& rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }
}
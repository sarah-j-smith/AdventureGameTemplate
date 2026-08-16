#include "Utils.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace Utils {
    std::vector<std::wstring> Split(const std::wstring &ToSplit, wchar_t Delimiter)
    {
        std::vector<std::wstring> tokens;
        std::wstring token;
        std::basic_istringstream<wchar_t> tokenStream(ToSplit);
        
        // Read up to the delimiter and push into the vector
        while (std::getline(tokenStream, token, Delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }

    std::wstring Join(const std::vector<std::wstring> &ToJoin, wchar_t Unifier)
    {
        std::basic_ostringstream<wchar_t> joined;
        wchar_t UnifierActual = wchar_t(0);
        for (auto S: ToJoin)
        {
            joined << S << UnifierActual;
            UnifierActual = Unifier;
        }
        return joined.str();
    }

    int Congruence(const std::vector<std::wstring> &A, const std::vector<std::wstring> &B)
    {
        auto IterB = B.cbegin();
        for (auto S: A)
        {
            if (S != *IterB)
            {
                return IterB - B.cbegin();
            }
        }
    }
};
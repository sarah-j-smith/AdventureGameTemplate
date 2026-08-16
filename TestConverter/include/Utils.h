#include <string>
#include <vector>

namespace Utils {
    std::vector<std::wstring> Split(const std::wstring &ToSplit, char Delimiter);
    std::wstring Join(const std::vector<std::wstring> &ToJoin, char Unifier);
    int Congruence(const std::vector<std::wstring> &A, const std::vector<std::wstring> &B);
}
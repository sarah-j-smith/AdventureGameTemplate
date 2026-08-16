#pragma once

#include <vector>
#include <string>
#include <memory>

class UnitNode {
public:
    std::wstring PathElement;
    std::vector<std::shared_ptr<UnitNode>> ChildNodes;
    bool IsLeaf = false;
    int TestCount;
    std::string TestDisplayName;
    int Warnings = 0;
    int Errors = 0;
};

#pragma once

#include <vector>

#include "nlohmann/json.hpp"
#include "UnitNode.h"

using Json = nlohmann::json;

class TestModel
{
public:
    std::shared_ptr<UnitNode> Suites;

    void AddData(Json Data);

    std::vector< std::vector< std::wstring >> Paths;
};

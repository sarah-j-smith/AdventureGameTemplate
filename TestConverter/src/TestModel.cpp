#include "TestModel.h"
#include "Utils.h"

#include <stdexcept>

void TestModel::AddData(Json Data)
{
    // Throws exceptions from the json library if any of the assumed data is wrong
    const Json Tests = Data["tests"];
    int Index = 0;
    for (Json Test: Tests)
    {
        const Json PathData = Test["fullTestPath"];
        const std::wstring Name = Test["testDisplayName"];
        Paths.push_back( Utils::Split(PathData, '.') );
    }

    // Find the suites - find the longest common prefix
    std::vector<std::wstring> Prefixes;

    // create root
    Suites = std::make_shared<UnitNode>();
    for (auto Path: Paths)
    {
        UnitNode *Ptr = Suites.get();
        for (auto PathElement: Path)
        {
            UnitNode *ChildPtr = nullptr;
            // Find node under current parent that matches this, or insert new
            for (auto Child: Ptr->ChildNodes)
            {
                if Child.
            }
        }
    }
    
}
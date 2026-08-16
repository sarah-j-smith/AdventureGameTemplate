#include <fstream>
#include <nlohmann/json.hpp>
#include <exception>
#include <iostream>

#define EXAMPLE_TEST_DATA_FILE_NAME "json/TestResultSample-2.json"

/** https://github.com/nlohmann/json  */
using Json = nlohmann::json;

#include "DartTestEvent.h"
#include "TestModel.h"

int main(int argc, char *argv[]) 
{
    const char *UnrealTestDataFileName;
    if (argc > 1) // 0 is prog name, 1 is 1st arg
    {
        UnrealTestDataFileName = argv[1];
    }
    else
    {
        UnrealTestDataFileName = EXAMPLE_TEST_DATA_FILE_NAME;
    }

    /// Read in the test data
    Json Data;
    std::ifstream UnrealTestDataFile(UnrealTestDataFileName);
    try {
        Data = Json::parse(UnrealTestDataFile);
    } catch(std::exception e) {
        std::cout << "Error reading file " << UnrealTestDataFileName << ": " << e.what() << std::endl;
    }

    // std::cout << data.dump(2) << std::endl;

    TestModel *Model = new TestModel();
    Model->AddData(Data);

    {
        // {"protocolVersion":"0.1.0","runnerVersion":"0.12.13+1","type":"start","time":0}
        StartEvent start;
        start.AddData(Data);
        std::wcout << start.Serialize() << std::endl;
    }

    {
        auto allSuites = std::make_shared<AllSuitesEvent>();
        allSuites->AddData(Data);
    }
}
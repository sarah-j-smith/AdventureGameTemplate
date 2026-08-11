#include <fstream>
#include <nlohmann/json.hpp>
#include <exception>
#include <iostream>

#define EXAMPLE_TEST_DATA_FILE_NAME "json/TestResultSample-2.json"

/** https://github.com/nlohmann/json  */
using json = nlohmann::json;

#include "DartTestEvent.h"


int main(int argc, char *argv[]) 
{
    const char *unrealTestDataFileName;
    if (argc > 1) // 0 is prog name, 1 is 1st arg
    {
        unrealTestDataFileName = argv[1];
    }
    else
    {
        unrealTestDataFileName = EXAMPLE_TEST_DATA_FILE_NAME;
    }

    /// Read in the test data
    json data;
    std::ifstream unrealTestDataFile(unrealTestDataFileName);
    try {
        data = json::parse(unrealTestDataFile);
    } catch(std::exception e) {
        std::cout << "Error reading file " << unrealTestDataFileName << ": " << e.what() << std::endl;
    }

    // std::cout << data.dump(2) << std::endl;

    {
        // {"protocolVersion":"0.1.0","runnerVersion":"0.12.13+1","type":"start","time":0}
        StartEvent start;
        start.AddData(data);
        std::cout << start.Serialize() << std::endl;
    }

    {
        auto allSuites = std::make_shared<AllSuitesEvent>();
        allSuites->AddData(data);
    }
}
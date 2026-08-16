#include "DartTestEvent.h"

#include <iostream>

#include <nlohmann/json.hpp>

///////////////////////////////////
///
///  EVENT
///

json Event::ToJson() const
{
    json j2 = {
        {"type", type},
        {"time", time}
    };
    return j2;
}

std::wstring Event::Serialize() const
{
    try {
        json j2 = ToJson();
        return j2.dump();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error in " << type << " - " << e.what() << '\n';
    }
    return std::wstring();
}

///////////////////////////////////
///
///  START EVENT
///

json StartEvent::ToJson() const
{
    json j2 = Event::ToJson();
    j2.update({{"protocolVersion", protocolVersion},
               {"runnerVersion", runnerVersion}});
    return j2;
}

void StartEvent::AddData(const json &Data)
{
    try
    {
        runnerVersion = Data["devices"][0]["platform"];
        if (Data["pid"].is_number_unsigned())
        {
            pid = Data["pid"];
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


///////////////////////////////////
///
///  ALL SUITES EVENT
///

json AllSuitesEvent::ToJson() const
{
    json j2 = Event::ToJson();
    j2.update({"count", count});
    return j2;
}

void AllSuitesEvent::AddData(const json &Data)
{
    try {
        // for now only support a single test suite - that can have groups and multiple tests
        count = 1;
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
}


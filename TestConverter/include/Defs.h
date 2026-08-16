#include <nlohmann/json.hpp>

/** https://github.com/nlohmann/json  */
using json = nlohmann::basic_json<std::map, std::vector, std::wstring>;

#define TXT(S) 

#define PROTOCOL_VERSION L"0.1.0"

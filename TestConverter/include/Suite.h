#include <string>
#include <optional>

class Suite {
  // An opaque ID for the suite.
  int id;

  // The platform on which the suite is running.
  std::string platform;

  // The path to the suite's file, or `null` if that path is unknown.
  std::optional<std::string> path;
};
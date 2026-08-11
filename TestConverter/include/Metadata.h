#include <string>
#include <optional>

class Metadata {
  bool skip;

  // The reason the tests was skipped, or `null` if it wasn't skipped.
  std::optional<std::string> skipReason;
};

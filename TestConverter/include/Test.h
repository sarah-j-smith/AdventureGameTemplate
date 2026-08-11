#include <string>
#include <vector>
#include <optional>

#include "Metadata.h"

class Test {
  // An opaque ID for the test.
  int id;

  // The name of the test, including prefixes from any containing groups.
  std::string name;

  // The ID of the suite containing this test.
  int suiteID;

  // The IDs of groups containing this test, in order from outermost to
  // innermost.
  std::vector<int> groupIDs;

  // The (1-based) line on which the test was defined, or `null`.
  std::optional<int> line;

  // The (1-based) column on which the test was defined, or `null`.
  std::optional<int> column;

  // The URL for the file in which the test was defined, or `null`.
  std::optional<std::string> url;

  // The (1-based) line in the original test suite from which the test
  // originated.
  //
  // Will only be present if `root_url` is different from `url`.
  std::optional<int> root_line;

  // The (1-based) line in the original test suite from which the test
  // originated.
  //
  // Will only be present if `root_url` is different from `url`.
  std::optional<int> root_column;

  // The URL for the original test suite in which the test was defined.
  //
  // Will only be present if different from `url`.
  std::optional<std::string> root_url;

  // This field is deprecated and should not be used.
  Metadata metadata;
};
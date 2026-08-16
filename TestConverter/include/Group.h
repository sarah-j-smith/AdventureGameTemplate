#pragma once

#include <string>
#include <optional>

#include "Metadata.h"

class Group {
  // An opaque ID for the group.
  int id;

  // The name of the group, including prefixes from any containing groups.
  std::wstring name;

  // The ID of the suite containing this group.
  int suiteID;

  // The ID of the group's parent group, unless it's the root group.
  std::optional<int> parentID;

  // The number of tests (recursively) within this group.
  int testCount;

  // The (1-based) line on which the group was defined, or `null`.
  std::optional<int> line;

  // The (1-based) column on which the group was defined, or `null`.
  std::optional<int> column;

  // The URL for the file in which the group was defined, or `null`.
  std::optional<std::wstring> url;

  // This field is deprecated and should not be used.
  Metadata metadata;
};

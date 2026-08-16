#pragma once

#include <string>
#include <optional>

class Suite {
  // An opaque ID for the suite.
  int id;

  // The platform on which the suite is running.
  std::wstring platform;

  // The path to the suite's file, or `null` if that path is unknown.
  std::optional<std::wstring> path;
};
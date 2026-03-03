#ifndef INPUT_H
#define INPUT_H

#include <cassert>
#include <string>
#include "./container/String.hpp"
#include "./container/vector.hpp"

namespace sjtu {

class Input {
public:

  void Skip();
  auto GetTimestamp() -> int;
  auto GetCommand() -> std::string;
  auto GetKey() -> char;
  auto GetChar() -> char;
  auto GetInteger() -> int;
  auto GetDate() -> int;
  auto GetTime() -> int;
  auto GetString() -> String;
  auto GetIntegerArray() -> vector<int>;
  auto GetStringArray() -> vector<String>;

private:
  char las_c_{'\n'};
};

}

#endif 
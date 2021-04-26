#pragma once
#include "../src/parser_combinator.h"
#include <functional>
#include <vector>
#include <string>

static Parser<int> calc = Parser<int>("calc", [](State &s) -> int {
  int x = natural.parse(s);
  symbol("+").parse(s);
  int y = natural.parse(s);
  return x + y;
});
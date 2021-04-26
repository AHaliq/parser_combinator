#include "./src/parser_combinator.h"
#include "./examples/calc.h"
#include <iostream>

int main() {
  std::cout << std::to_string(calc.parse(" 3123  + 1000")) << "\n";
  return 0;
}
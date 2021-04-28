#include "catch.hpp"
#include "parser_combinator.h"
#include <string>

using namespace parser;

TEST_CASE("state tests") {
  std::string str = "hello";
  State s(&str);
  REQUIRE(s.adv() == 'h');
  REQUIRE(s.adv() == 'e');
  REQUIRE(s.adv() == 'l');
  REQUIRE(s.adv() == 'l');
  REQUIRE(s.adv() == 'o');
  REQUIRE_THROWS(s.adv());
}
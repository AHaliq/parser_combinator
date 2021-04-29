#include "catch.hpp"
#include "fakeit.hpp"
#include "parser_combinator.h"
#include <functional>
#include <string>
#include <vector>

using namespace fakeit;
using State = parser::state::State<>;
using StateString = parser::state::StateString<>;

TEST_CASE("parser base class") {
  SECTION("core function success") {
    std::string str = "kool";
    StateString s1(&str);
    StateString s2(&str);
    
    parser::Parser<char> p1("test", [](State &s) -> char { return 'k'; });
    REQUIRE(p1.parse(s1) == 'k');
  }
  SECTION("core function failure") {
    std::string str = "kool";
    StateString s2(&str);
    parser::Parser<char> p2("test", [](State &s) -> char { throw std::vector<State>(); });
    try {
      p2.parse(s2);
      REQUIRE(false);
    } catch (std::vector<State> &e) {
      REQUIRE(e[0].has_failed());
      REQUIRE(e[0].get_fail() == "test");
    }
  }
}
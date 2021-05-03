#include "catch.hpp"
#include "parser_combinator.h"
#include <string>
#include <memory>

using namespace parser;
using namespace parser::state;
using namespace parser::maps;
using namespace parser::combiners;
using namespace parser::alg;
using namespace parser::primitives;

template <typename T>
T parse(std::shared_ptr<Parser<T>> p, std::string str) {
  StateString<> s(&str);
  return p->parse(s);
}

TEST_CASE("basic primitive parsers") {
  SECTION("id") {
    REQUIRE(parse(id<>, "hello") == 0);
  }
  SECTION("item") {
    REQUIRE(parse(item<>, "hello") == 'h');
    REQUIRE(parse(item<>++->map<std::string>(str_of_charvec), "hello") == "hello");
  }
  SECTION("sat") {
    REQUIRE(parse(sat<>(digit_pred), "123") == '1');
    REQUIRE(parse(sat<>(lower_pred), "abc") == 'a');
    REQUIRE(parse(sat<>(upper_pred), "Abc") == 'A');
    REQUIRE(parse(sat<>(letter_pred)++->map<std::string>(str_of_charvec), "gA") == "gA");
    REQUIRE(parse(sat<>(alphanum_pred)++->map<std::string>(str_of_charvec), "1a") == "1a");
    REQUIRE(parse(sat<>(space_pred), " ") == ' ');
  }
}
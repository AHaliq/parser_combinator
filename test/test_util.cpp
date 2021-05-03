#include "catch.hpp"
#include "parser_combinator.h"

using namespace parser::maps;
using namespace parser::combiners;
using namespace parser::alg;

TEST_CASE("maps")
{
  SECTION("both mappers")
  {
    Both<int, char> b(1, 'a');
    Both<Both<int, char>, int> b1(b, 2);
    Both<int, Both<int, char>> b2(2, b);
    REQUIRE(fst(b) == 1);
    REQUIRE(snd(b) == 'a');
    REQUIRE(get_mid(b1) == 'a');
    REQUIRE(get_mid(b2) == 1);
  }
  SECTION("either mappers")
  {
    Either<int, int> l = Left<int, int>(1);
    Either<int, int> r = Right<int, int>(2);
    REQUIRE(get_either(l) == 1);
    REQUIRE(get_either(r) == 2);
  }
  SECTION("either all mappers")
  {
    EitherAll<int, char> b = Left<Both<int, char>, Either<int, char>>(Both<int, char>(1, 'a'));
    EitherAll<int, char> l = Right<Both<int, char>, Either<int, char>>(Left<int, char>(1));
    EitherAll<int, char> r = Right<Both<int, char>, Either<int, char>>(Right<int, char>('a'));
    REQUIRE(first_exists(b));
    REQUIRE(second_exists(b));
    REQUIRE(get_first(b) == 1);
    REQUIRE(get_second(b) == 'a');
    REQUIRE(first_exists(l));
    REQUIRE_FALSE(second_exists(l));
    REQUIRE(get_first(l) == 1);
    REQUIRE_FALSE(first_exists(r));
    REQUIRE(second_exists(r));
    REQUIRE(get_second(r) == 'a');
  }
}

TEST_CASE("combiners")
{
  SECTION("primitives") {
    REQUIRE(str_of_chars('a','b') == "ab");
    REQUIRE(str_concat("hello","there") == "hellothere");
  }
  SECTION("vectors") {
    std::vector<int> xs{ 1, 2, 3};
    std::function<int(int,int)> mul = [](int a, int b) { return a * b; };
    REQUIRE(fmap(mul)(xs, 2) == std::vector<int>{2,4,6});
    REQUIRE(product(mul)(xs,xs) == std::vector<int>{1,2,3,2,4,6,3,6,9});
  }
}
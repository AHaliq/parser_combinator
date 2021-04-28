#include "catch.hpp"
#include "parser_combinator.h"

TEST_CASE("either construction") {
  SECTION("primtive literals") {
    Either<int,int> e = Left<int,int>(1);
    REQUIRE(e.lx == 1);
    REQUIRE(e.left);
  }
}
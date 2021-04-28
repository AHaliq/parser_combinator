#include "catch.hpp"
#include "parser_combinator.h"

struct ent {
  int x;
};

class cls {
public:
  int x;
  cls() {}
  cls(int v) : x(v) {}
};

TEST_CASE("either construction") {
  SECTION("primtive literals") {
    Either<int,int> e = Left<int,int>(1);
    REQUIRE(e.lx == 1);
    REQUIRE(e.left);
    e = Right<int,int>(2);
    REQUIRE(e.rx == 2);
    REQUIRE_FALSE(e.left);
  }
  SECTION("struct literals") {
    ent x{ 1 };
    Either<ent,ent> e = Left<ent,ent>(x);
    REQUIRE(e.lx.x == 1);
    REQUIRE(e.left);
    e = Right<ent,ent>(x);
    REQUIRE(e.rx.x == 1);
    REQUIRE_FALSE(e.left);
  }
  SECTION("class literals") {
    cls x(1);
    Either<cls,cls> e = Left<cls,cls>(x);
    REQUIRE(e.lx.x == 1);
    REQUIRE(e.left);
    e = Right<cls,cls>(x);
    REQUIRE(e.rx.x == 1);
    REQUIRE_FALSE(e.left);
  }
  SECTION("primitive pointer") {
    int x = 1, *xp = &x;
    Either<int*,int*> e = Left<int*,int*>(xp);
    REQUIRE(e.lx == xp);
    REQUIRE(*e.lx == 1);
    REQUIRE(e.left);
    e = Right<int*,int*>(xp);
    REQUIRE(e.rx == xp);
    REQUIRE(*e.rx == 1);
    REQUIRE_FALSE(e.left);
  }
  SECTION("struct pointer") {
    ent x{1}, *xp = &x;
    Either<ent*,ent*> e = Left<ent*,ent*>(xp);
    REQUIRE(e.lx->x == 1);
    REQUIRE(e.lx == xp);
    REQUIRE(e.left);
    e = Right<ent*,ent*>(xp);
    REQUIRE(e.rx->x == 1);
    REQUIRE(e.rx == xp);
    REQUIRE_FALSE(e.left);
  }
  SECTION("class pointer") {
    cls x(1), *xp = &x;
    Either<cls*,cls*> e = Left<cls*,cls*>(xp);
    REQUIRE(e.lx->x == 1);
    REQUIRE(e.lx == xp);
    REQUIRE(e.left);
    e = Right<cls*,cls*>(xp);
    REQUIRE(e.rx->x == 1);
    REQUIRE(e.rx == xp);
    REQUIRE_FALSE(e.left);
  }
}
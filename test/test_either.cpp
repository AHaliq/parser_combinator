#include "catch.hpp"
#include "parser_combinator.h"

using namespace alg;
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
    REQUIRE(util::get_either<int>(e) == 1);
    e = Right<int,int>(2);
    REQUIRE(e.rx == 2);
    REQUIRE_FALSE(e.left);
    REQUIRE(util::get_either<int>(e) == 2);
  }
  SECTION("struct literals") {
    ent x{ 1 };
    Either<ent,ent> e = Left<ent,ent>(x);
    REQUIRE(e.lx.x == 1);
    REQUIRE(e.left);
    REQUIRE(util::get_either<ent>(e).x == 1);
    e = Right<ent,ent>(x);
    REQUIRE(e.rx.x == 1);
    REQUIRE_FALSE(e.left);
    REQUIRE(util::get_either<ent>(e).x == 1);
  }
  SECTION("class literals") {
    cls x(1);
    Either<cls,cls> e = Left<cls,cls>(x);
    REQUIRE(e.lx.x == 1);
    REQUIRE(e.left);
    REQUIRE(util::get_either<cls>(e).x == 1);
    e = Right<cls,cls>(x);
    REQUIRE(e.rx.x == 1);
    REQUIRE_FALSE(e.left);
    REQUIRE(util::get_either<cls>(e).x == 1);
  }
  SECTION("primitive pointer") {
    int x = 1, *xp = &x;
    Either<int*,int*> e = Left<int*,int*>(xp);
    REQUIRE(e.lx == xp);
    REQUIRE(*e.lx == 1);
    REQUIRE(e.left);
    REQUIRE(util::get_either<int*>(e) == xp);
    REQUIRE(*util::get_either<int*>(e) == 1);
    e = Right<int*,int*>(xp);
    REQUIRE(e.rx == xp);
    REQUIRE(*e.rx == 1);
    REQUIRE_FALSE(e.left);
    REQUIRE(util::get_either<int*>(e) == xp);
    REQUIRE(*util::get_either<int*>(e) == 1);
  }
  SECTION("struct pointer") {
    ent x{1}, *xp = &x;
    Either<ent*,ent*> e = Left<ent*,ent*>(xp);
    REQUIRE(e.lx->x == 1);
    REQUIRE(e.lx == xp);
    REQUIRE(e.left);
    REQUIRE(util::get_either<ent*>(e) == xp);
    REQUIRE(util::get_either<ent*>(e)->x == 1);
    e = Right<ent*,ent*>(xp);
    REQUIRE(e.rx->x == 1);
    REQUIRE(e.rx == xp);
    REQUIRE_FALSE(e.left);
    REQUIRE(util::get_either<ent*>(e) == xp);
    REQUIRE(util::get_either<ent*>(e)->x == 1);
  }
  SECTION("class pointer") {
    cls x(1), *xp = &x;
    Either<cls*,cls*> e = Left<cls*,cls*>(xp);
    REQUIRE(e.lx->x == 1);
    REQUIRE(e.lx == xp);
    REQUIRE(e.left);
    REQUIRE(util::get_either<cls*>(e) == xp);
    REQUIRE(util::get_either<cls*>(e)->x == 1);
    e = Right<cls*,cls*>(xp);
    REQUIRE(e.rx->x == 1);
    REQUIRE(e.rx == xp);
    REQUIRE_FALSE(e.left);
    REQUIRE(util::get_either<cls*>(e) == xp);
    REQUIRE(util::get_either<cls*>(e)->x == 1);
  }
}
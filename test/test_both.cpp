#include "catch.hpp"
#include "parser_combinator.h"

using namespace parser::alg;
using namespace parser::maps;
struct ent {
  int x;
};

class cls {
public:
  int x;
  cls() {}
  cls(int v) : x(v) {}
};

TEST_CASE("basic construction") {
  SECTION("primitives literal") {
    Both<int,int> b(1,2);
    REQUIRE(b.lx == 1);
    REQUIRE(b.rx == 2);
  }
  SECTION("structs literal") {
    Both<ent,ent> b((struct ent){ 1 },(struct ent){ 2 });
    REQUIRE(b.lx.x == 1);
    REQUIRE(b.rx.x == 2);
  }
  SECTION("class literals") {
    Both<cls, cls> b(cls(1), cls(2));
    REQUIRE(b.lx.x == 1);
    REQUIRE(b.rx.x == 2);
  }
  SECTION("primtives pointer") {
    int x = 1, y = 2;
    int *xp = &x, *yp = &y;
    Both<int*,int*> b(xp, yp);
    REQUIRE(b.lx == xp);
    REQUIRE(b.rx == yp);
  }
  SECTION("structs pointer") {
    ent *x = new (struct ent){ 1 };
    ent *y = new (struct ent){ 2 };
    Both<ent*, ent*> b(x,y);
    REQUIRE(b.lx == x);
    REQUIRE(b.rx == y);
    delete x;
    delete y;
  }
  SECTION("class pointer") {
    cls *x = new cls(1);
    cls *y = new cls(2);
    Both<cls*, cls*> b(x, y);
    REQUIRE(b.lx == x);
    REQUIRE(b.rx == y);
    delete x;
    delete y;
  }
}

TEST_CASE("closure construction") {
  SECTION("factory function") {
    Both<int,int*> b = ([]() {
      int *yp = new int(2);
      return Both<int,int*>(1,yp);
    })();
    REQUIRE(b.lx == 1);
    REQUIRE(*b.rx == 2);
    delete b.rx;
  }
}

TEST_CASE("both getters") {
  Both<int, std::string> b(1, "hey");
  REQUIRE(fst(b) == 1);
  REQUIRE(snd(b) == "hey");
}

TEST_CASE("nested both") {
  SECTION("nested pointer both") {
    Both<int,int> *b1 = new Both<int,int>(1,2);
    Both<Both<int,int>*,int> b2(b1, 3);
    REQUIRE(b2.lx->lx == 1);
    REQUIRE(b2.lx->rx == 2);
    REQUIRE(b2.lx == b1);
    REQUIRE(b2.rx == 3);
    delete b1;
  }
  SECTION("get mid") {
    Both<int,int> b3(1,2);
    Both<Both<int,int>,int> b4(b3,3);
    REQUIRE(get_mid(b4) == 2);
  }
}
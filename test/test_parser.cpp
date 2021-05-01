#include "catch.hpp"
#include "fakeit.hpp"
#include "parser_combinator.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

using namespace fakeit;
using State = parser::state::State<>;
using StateString = parser::state::StateString<>;
template <typename T>
using P = parser::Parser<T>;
template <typename T>
using PP = std::shared_ptr<parser::Parser<T>>;

using empty_md = std::vector<parser::ParserMetaData const*>;

TEST_CASE("metadata") {
  SECTION("metadata user") {
    P<char> p("test", [](State &s) -> char { return 'k'; });
    REQUIRE(p.metadata.type == parser::USER);
    REQUIRE(p.metadata.user_label == "test");
    REQUIRE(p.metadata.children == empty_md());
  }
  SECTION("metadata non user") {
    P<char> p(parser::SEQ, [](State &s) -> char { return 'k'; });
    REQUIRE(p.metadata.type == parser::SEQ);
    REQUIRE(p.metadata.children == empty_md());
  }
}

TEST_CASE("parser base class") {
  SECTION("core function success") {
    std::string str = "kool";
    StateString s(&str);
    P<char> p("test", [](State &s) { return 'k'; });
    REQUIRE(p.parse(s) == 'k');
  }
  SECTION("core function failure") {
    std::string str = "kool";
    StateString s(&str);
    P<char> p("test", [](State &s) -> char { throw std::vector<State>(); });
    try {
      p.parse(s);
      REQUIRE(false);
    } catch (std::vector<State> &e) {
      REQUIRE(e[0].has_failed());
      REQUIRE(e[0].get_fail() == p.metadata.uuid);
    }
  }
}

TEST_CASE("parser seq") {
  SECTION("seq general") {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
      "test2", [](State &s) { return 'k'; });
    PP<std::string> p3 = p1.seq<char,std::string>(
      p2, [](char a, char b) { return std::string(1,a) + std::string(1,b); });

    State s;
    REQUIRE(p3->parse(s) == "ok");
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both") {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
      "test2", [](State &s) { return 'k'; });
    PP<parser::alg::Both<char,char>> p3 = p1.seq(p2);

    State s;
    parser::alg::Both<char,char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both class method operator") {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
      "test2", [](State &s) -> char { return 'k'; });
    PP<parser::alg::Both<char,char>> p3 = p1 & p2;

    State s;
    parser::alg::Both<char,char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both shared_ptr operator") {
    PP<char> p1 = std::make_shared<P<char>>(
      "test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
      "test2", [](State &s) { return 'k'; });
    PP<parser::alg::Both<char,char>> p3 = p1 & p2;

    State s;
    parser::alg::Both<char,char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1->metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
}

TEST_CASE("parser alt") {
  SECTION("alt general both") {
    
  }
  SECTION("alt general left") {

  }
  SECTION("alt general right") {

  }
  SECTION("alt same type both") {

  }
  SECTION("alt same type left") {

  }
  SECTION("alt same type right") {

  }
  SECTION("alt same type operator") {

  }
}
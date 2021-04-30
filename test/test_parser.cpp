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

TEST_CASE("metadata") {
  SECTION("metadata user") {
    parser::Parser<char> p("test", [](State &s) -> char { return 'k'; });
    REQUIRE(p.metadata.type == parser::USER);
    REQUIRE(p.metadata.user_label == "test");
    REQUIRE(p.metadata.children == std::vector<parser::ParserMetaData const*>());
  }
  SECTION("metadata non user") {
    parser::Parser<char> p(parser::SEQ, [](State &s) -> char { return 'k'; });
    REQUIRE(p.metadata.type == parser::SEQ);
    REQUIRE(p.metadata.children == std::vector<parser::ParserMetaData const*>());
  }
}

TEST_CASE("parser base class") {
  SECTION("core function success") {
    std::string str = "kool";
    StateString s(&str);
    parser::Parser<char> p("test", [](State &s) -> char { return 'k'; });
    REQUIRE(p.parse(s) == 'k');
  }
  SECTION("core function failure") {
    std::string str = "kool";
    StateString s(&str);
    parser::Parser<char> p("test", [](State &s) -> char { throw std::vector<State>(); });
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
    parser::Parser<char> p1("test1", [](State &s) -> char { return 'o'; });
    std::shared_ptr<parser::Parser<char>> p2 = std::make_shared<parser::Parser<char>>("test2", [](State &s) -> char { return 'k'; });
    std::shared_ptr<parser::Parser<std::string>> p3 = p1.seq<char,std::string>(p2, [](char a, char b) -> std::string { return std::string(1,a) + std::string(1,b); });

    State s;
    REQUIRE(p3->parse(s) == "ok");
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both") {
    parser::Parser<char> p1("test1", [](State &s) -> char { return 'o'; });
    std::shared_ptr<parser::Parser<char>> p2 = std::make_shared<parser::Parser<char>>("test2", [](State &s) -> char { return 'k'; });
    std::shared_ptr<parser::Parser<parser::alg::Both<char,char>>> p3 = p1.seq(p2);

    State s;
    parser::alg::Both<char,char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both operator") {
    parser::Parser<char> p1("test1", [](State &s) -> char { return 'o'; });
    std::shared_ptr<parser::Parser<char>> p2 = std::make_shared<parser::Parser<char>>("test2", [](State &s) -> char { return 'k'; });
    std::shared_ptr<parser::Parser<parser::alg::Both<char,char>>> p3 = p1 > p2;

    State s;
    parser::alg::Both<char,char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
}
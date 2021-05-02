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

using empty_md = std::vector<parser::ParserMetaData const *>;

TEST_CASE("metadata")
{
  SECTION("metadata user")
  {
    P<char> p("test", [](State &s) -> char { return 'k'; });
    REQUIRE(p.metadata.type == parser::USER);
    REQUIRE(p.metadata.user_label == "test");
    REQUIRE(p.metadata.children == empty_md());
  }
  SECTION("metadata non user")
  {
    P<char> p(parser::SEQ, [](State &s) -> char { return 'k'; });
    REQUIRE(p.metadata.type == parser::SEQ);
    REQUIRE(p.metadata.children == empty_md());
  }
}

TEST_CASE("parser base class")
{
  SECTION("core function success")
  {
    std::string str = "kool";
    StateString s(&str);
    P<char> p("test", [](State &s) { return 'k'; });
    REQUIRE(p.parse(s) == 'k');
  }
  SECTION("core function failure")
  {
    std::string str = "kool";
    StateString s(&str);
    P<char> p("test", [](State &s) -> char { throw std::vector<State>(); });
    try
    {
      p.parse(s);
      REQUIRE(false);
    }
    catch (std::vector<State> &e)
    {
      REQUIRE(e[0].has_failed());
      REQUIRE(e[0].get_fail() == p.metadata.uuid);
    }
  }
}

TEST_CASE("parser seq")
{
  SECTION("seq general")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<std::string> p3 = p1.seq<char, std::string>(
        p2, [](char a, char b) { return std::string(1, a) + std::string(1, b); });

    State s;
    REQUIRE(p3->parse(s) == "ok");
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<parser::alg::Both<char, char>> p3 = p1.seq(p2);

    State s;
    parser::alg::Both<char, char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore fst") {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1.seq_ignore_fst(p2);

    State s;
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore fst operator") {
    PP<char> p1 = std::make_shared<P<char>>(
        "test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1 > p2;

    State s;
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &p1->metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore snd") {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1.seq_ignore_snd(p2);

    State s;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore snd operator") {
    PP<char> p1 = std::make_shared<P<char>>(
        "test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1 < p2;

    State s;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &p1->metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both class method operator")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) -> char { return 'k'; });
    PP<parser::alg::Both<char, char>> p3 = p1 & p2;

    State s;
    parser::alg::Both<char, char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both shared_ptr operator")
  {
    PP<char> p1 = std::make_shared<P<char>>(
        "test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<parser::alg::Both<char, char>> p3 = p1 & p2;

    State s;
    parser::alg::Both<char, char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1->metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
}

TEST_CASE("parser alt")
{
  SECTION("alt general both")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<parser::alg::Either<char, char>> p3 = p1.alt<char>(p2);

    State s;
    parser::alg::Either<char, char> res = p3->parse(s);
    REQUIRE(res.left);
    REQUIRE(res.lx == 'o');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt general left")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) -> char { throw std::vector<State>(); });
    PP<parser::alg::Either<char, char>> p3 = p1.alt<char>(p2);

    State s;
    parser::alg::Either<char, char> res = p3->parse(s);
    REQUIRE(res.left);
    REQUIRE(res.lx == 'o');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt general right")
  {
    P<char> p1("test1", [](State &s) -> char { throw std::vector<State>(); });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<parser::alg::Either<char, char>> p3 = p1.alt<char>(p2);

    State s;
    parser::alg::Either<char, char> res = p3->parse(s);
    REQUIRE_FALSE(res.left);
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type both")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1.alt(p2);

    State s;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type left")
  {
    P<char> p1("test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) -> char { throw std::vector<State>(); });
    PP<char> p3 = p1.alt(p2);

    State s;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type right")
  {
    P<char> p1("test1", [](State &s) -> char { throw std::vector<State>(); });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1.alt(p2);

    State s;
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type method operator")
  {
    P<char> p1("test1", [](State &s) -> char { throw std::vector<State>(); });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) { return 'k'; });
    PP<char> p3 = p1 | p2;

    State s;
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &p1.metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type shared_ptr operator")
  {
    PP<char> p1 = std::make_shared<P<char>>(
        "test1", [](State &s) { return 'o'; });
    PP<char> p2 = std::make_shared<P<char>>(
        "test2", [](State &s) -> char { throw std::vector<State>(); });
    PP<char> p3 = p1 | p2;

    State s;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &p1->metadata);
    REQUIRE(p3->metadata.children[1] == &p2->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
}

TEST_CASE("parser many")
{
  State s;
  int count = 0;
  PP<char> p1 = std::make_shared<P<char>>(
      "test1", [&count](State &s) {
        if (count < 3)
        {
          count++;
          return 'a';
        }
        else
        {
          throw std::vector<State>();
        }
      });
  SECTION("many fold success")
  {
    count = 0;
    std::string str = "";
    REQUIRE(p1->many<std::string>(
                  [](char c, std::string st) {
                    return st + c;
                  },
                  "")
                ->parse(s) == "aaa");
  }
  SECTION("many fold fail")
  {
    count = 0;
    std::string str = "";
    REQUIRE_THROWS(p1->many<std::string>(
                  [](char c, std::string st) {
                    return st + c;
                  },
                  "", 5)
                ->parse(s));
  }
  SECTION("many vector success")
  {
    count = 0;
    REQUIRE(p1->many()->parse(s) == std::vector<char>{'a','a','a'});
  }
  SECTION("many vector fail")
  {
    count = 0;
    REQUIRE_THROWS(p1->many(5)->parse(s));
  }
  SECTION("some success")
  {
    count = 0;
    REQUIRE(p1->some()->parse(s) == std::vector<char>{'a','a','a'});
  }
  SECTION("some fail")
  {
    count = 5;
    REQUIRE_THROWS(p1->some()->parse(s));
  }
  SECTION("many vector operator")
  {
    count = 0;
    REQUIRE((p1 + 3) ->parse(s) == std::vector<char>{'a','a','a'});
  }
  SECTION("some operator")
  {
    count = 0;
    REQUIRE(p1 ++ ->parse(s) == std::vector<char>{'a','a','a'});
  }
}

TEST_CASE("map parser") {
  State s;
  P<int> p1("test1", [](State &s) { return 1; });
  PP<int> p2 = std::make_shared<P<int>>("test2", [](State &s) { return 3; });
  std::function<int(int)> inc = [](int x) -> int { return x + 1; };
  SECTION("base map") {
    REQUIRE(p1.map(inc)->parse(s) == 2);
  }
  SECTION("map operator method") {
    REQUIRE((p2 % inc) -> parse(s) == 4);
  }
}
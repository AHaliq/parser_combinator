#include "catch.hpp"
#include "fakeit.hpp"
#include "parser_combinator.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

using namespace fakeit;
using namespace parser::alg;
using State = parser::state::State<>;
using StateString = parser::state::StateString<>;
template <typename T>
using P = parser::Parser<T>;
template <typename T>
using PP = std::shared_ptr<parser::Parser<T>>;

using empty_md = std::vector<parser::ParserMetaData const *>;

State s;
std::function<char(State&)> o_func =
  [](State &s) { return 'o'; };
std::function<char(State&)> k_func =
  [](State &s) { return 'k'; };
std::function<int(State&)> one_func =
  [](State &s) { return 1; };
std::function<char(State&)> inv_func =
  [](State &s) -> char { throw std::vector<State>(); };
std::function<int(State&)> inv_func_int =
  [](State &s) -> int { throw std::vector<State>(); };
std::function<std::string(char,char)> char2str =
  [](char a, char b) { return std::string(1, a) + std::string(1, b); };
P<char> po("o_parser", o_func);
P<char> pk("k_parser", k_func);
P<int> p1("1_parser", one_func);
P<char> pinv("inv", inv_func);
P<int> pinvi("invi", inv_func_int);
PP<char> ppo = std::make_shared<P<char>>("o_parser", o_func);
PP<char> ppk = std::make_shared<P<char>>("k_parser", k_func);
PP<int> pp1 = std::make_shared<P<int>>("1_parser", one_func);
PP<char> ppinv = std::make_shared<P<char>>("inv", inv_func);
PP<int> ppinvi = std::make_shared<P<int>>("invi", inv_func_int);

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
    PP<std::string> p3 = po.seq(ppk, char2str);
    REQUIRE(p3->parse(s) == "ok");
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq general fail") {
    //TODO
  }
  SECTION("seq both")
  {
    PP<Both<char, char>> p3 = po.seq(ppk);
    Both<char, char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both fail") {
    //TODO
  }
  SECTION("seq ignore fst") {
    PP<char> p3 = po.seq_ignore_fst(ppk);
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore fst fail") {
    //TODO
  }
  SECTION("seq ignore fst operator") {
    PP<char> p3 = ppo > ppk;
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &ppo->metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore snd") {
    PP<char> p3 = po.seq_ignore_snd(ppk);
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq ignore snd fail") {
    //TODO
  }
  SECTION("seq ignore snd operator") {
    PP<char> p3 = ppo < ppk;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &ppo->metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both class method operator")
  {
    PP<Both<char, char>> p3 = po & ppk;
    Both<char, char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
  SECTION("seq both shared_ptr operator")
  {
    PP<Both<char, char>> p3 = ppo & ppk;
    Both<char, char> res = p3->parse(s);
    REQUIRE(res.lx == 'o');
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &ppo->metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::SEQ);
  }
}

TEST_CASE("parser alt")
{
  SECTION("alt general both")
  {
    PP<Either<char, char>> p3 = po.alt<char>(ppk);
    Either<char, char> res = p3->parse(s);
    REQUIRE(res.left);
    REQUIRE(res.lx == 'o');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt general left")
  {
    PP<Either<char, char>> p3 = po.alt<char>(ppinv);
    Either<char, char> res = p3->parse(s);
    REQUIRE(res.left);
    REQUIRE(res.lx == 'o');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppinv->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt general right")
  {
    PP<Either<char, char>> p3 = pinv.alt<char>(ppk);
    Either<char, char> res = p3->parse(s);
    REQUIRE_FALSE(res.left);
    REQUIRE(res.rx == 'k');
    REQUIRE(p3->metadata.children[0] == &pinv.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt general fail") {
    
  }
  SECTION("alt same type both")
  {
    PP<char> p3 = po.alt(ppk);
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type left")
  {
    PP<char> p3 = po.alt(ppinv);
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &po.metadata);
    REQUIRE(p3->metadata.children[1] == &ppinv->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type right")
  {
    PP<char> p3 = pinv.alt(ppk);
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &pinv.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type fail") {
    
  }
  SECTION("alt both different type both") {
    PP<EitherAll<char,int>> p3 = po.alt_both<int>(pp1);
    EitherAll<char,int> res = p3->parse(s);
    REQUIRE(first_exists(res));
    REQUIRE(second_exists(res));
    REQUIRE(get_first(res) == 'o');
    REQUIRE(get_second(res) == 1);
  }
  SECTION("alt both different type left") {
    PP<EitherAll<char,int>> p3 = po.alt_both<int>(ppinvi);
    EitherAll<char,int> res = p3->parse(s);
    REQUIRE(first_exists(res));
    REQUIRE_FALSE(second_exists(res));
    REQUIRE(get_first(res) == 'o');
  }
  SECTION("alt both different type right") {
    PP<EitherAll<char,int>> p3 = pinv.alt_both<int>(pp1);
    EitherAll<char,int> res = p3->parse(s);
    REQUIRE_FALSE(first_exists(res));
    REQUIRE(second_exists(res));
    REQUIRE(get_second(res) == 1);
  }
  SECTION("alt both different type fail") {
    //TODO
  }
  SECTION("alt both same type both") {
    //TODO
  }
  SECTION("alt both same type left") {
    //TODO
  }
  SECTION("alt both same type right") {
    //TODO 
  }
  SECTION("alt both same type fail") {
    //TODO
  }
  SECTION("alt same type method operator")
  {
    PP<char> p3 = pinv | ppk;
    REQUIRE(p3->parse(s) == 'k');
    REQUIRE(p3->metadata.children[0] == &pinv.metadata);
    REQUIRE(p3->metadata.children[1] == &ppk->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt same type shared_ptr operator")
  {
    PP<char> p3 = ppo | ppinv;
    REQUIRE(p3->parse(s) == 'o');
    REQUIRE(p3->metadata.children[0] == &ppo->metadata);
    REQUIRE(p3->metadata.children[1] == &ppinv->metadata);
    REQUIRE(p3->metadata.type == parser::ALT);
  }
  SECTION("alt both same type operator") {
    //TODO
  }
  SECTION("alt both same type vector chaining operator") {
    //TODO
  }
  SECTION("alt both same type vector chaining operator fail") {
    //TODO
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
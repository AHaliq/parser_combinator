#include "catch.hpp"
#include "parser_combinator.h"
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>

using State = parser::state::State<>;
using StateString = parser::state::StateString<>;
using StateIStream = parser::state::StateIStream<>;
using StateStreamBuf = parser::state::StateStreamBuf<>;

TEST_CASE("state base class")
{
  State s;
  REQUIRE_THROWS(s.adv());
  REQUIRE_FALSE(s.has_failed());
  REQUIRE(s.get_fail() == -1);
  s.fail(123);
  REQUIRE(s.has_failed());
  REQUIRE(s.get_fail() == 123);
}

TEST_CASE("state string")
{
  SECTION("standard parse")
  {
    std::string str = "hello";
    StateString s(&str);
    REQUIRE(s.adv() == 'h');
    REQUIRE(s.adv() == 'e');
    REQUIRE(s.adv() == 'l');
    REQUIRE(s.adv() == 'l');
    REQUIRE(s.adv() == 'o');
    REQUIRE_THROWS(s.adv());
  }
  SECTION("jump parse")
  {
    std::string str = "hello";
    StateString s1(&str);
    REQUIRE(s1.adv() == 'h');
    StateString s2 = s1;
    REQUIRE(s1.adv() == 'e');
    s1 = s2;
    REQUIRE(s1.adv() == 'e');
  }
}

TEST_CASE("state string stream")
{
  SECTION("standard parse")
  {
    std::string str = "hello";
    std::stringstream stream(str);
    StateIStream s(&stream);
    REQUIRE(s.adv() == 'h');
    REQUIRE(s.adv() == 'e');
    REQUIRE(s.adv() == 'l');
    REQUIRE(s.adv() == 'l');
    REQUIRE(s.adv() == 'o');
    REQUIRE_THROWS(s.adv());
  }
  SECTION("jump parse")
  {
    std::string str = "hello";
    std::stringstream stream(str);
    StateIStream s1(&stream);
    REQUIRE(s1.adv() == 'h');
    StateIStream s2 = s1;
    REQUIRE(s1.adv() == 'e');
    s1 = s2;
    REQUIRE(s1.adv() == 'e');
  }
}

TEST_CASE("state file stream")
{
  SECTION("standard parse")
  {
    std::fstream file;
    file.open("test.txt", std::fstream::out);
    file << "hello";
    file.close();
    file.open("test.txt");
    StateIStream s(&file);
    REQUIRE(s.adv() == 'h');
    REQUIRE(s.adv() == 'e');
    REQUIRE(s.adv() == 'l');
    REQUIRE(s.adv() == 'l');
    REQUIRE(s.adv() == 'o');
    REQUIRE_THROWS(s.adv());
    file.close();
    remove("test.txt");
  }
  SECTION("jump parse")
  {
    std::fstream file;
    file.open("test.txt", std::fstream::out);
    file << "hello";
    file.close();
    file.open("test.txt");
    StateIStream s1(&file);
    REQUIRE(s1.adv() == 'h');
    StateIStream s2 = s1;
    REQUIRE(s1.adv() == 'e');
    s1 = s2;
    REQUIRE(s1.adv() == 'e');
    file.close();
    remove("test.txt");
  }
}

TEST_CASE("state streambuf")
{
  SECTION("standard parse")
  {
    const char sentence[] = "hello";
    std::filebuf pbuf;
    pbuf.open("test.txt", std::ios_base::out);
    pbuf.sputn(sentence, sizeof(sentence) - 1);
    pbuf.close();
    pbuf.open("test.txt", std::ios_base::in);
    StateStreamBuf s1(&pbuf);
    REQUIRE(s1.adv() == 'h');
    REQUIRE(s1.adv() == 'e');
    REQUIRE(s1.adv() == 'l');
    REQUIRE(s1.adv() == 'l');
    REQUIRE(s1.adv() == 'o');
    REQUIRE_THROWS(s1.adv());
    pbuf.close();
    remove("test.txt");
  }
  SECTION("jump parse")
  {
    const char sentence[] = "hello";
    std::filebuf pbuf;
    pbuf.open("test.txt", std::ios_base::out);
    pbuf.sputn(sentence, sizeof(sentence) - 1);
    pbuf.close();
    pbuf.open("test.txt", std::ios_base::in);
    StateStreamBuf s1(&pbuf);
    REQUIRE(s1.adv() == 'h');
    StateStreamBuf s2 = s1;
    REQUIRE(s1.adv() == 'e');
    s1 = s2;
    REQUIRE(s1.adv() == 'e');
    pbuf.close();
    remove("test.txt");
  }
}
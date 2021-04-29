#include "catch.hpp"
#include "parser_combinator.h"
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>

using namespace parser::state;

TEST_CASE("state base class") {
  State s;
  REQUIRE(s.adv() == 0);
  REQUIRE_FALSE(s.has_failed());
  REQUIRE(s.get_fail() == STATE_NOT_FAILED_LABEL);
  s.fail("test");
  REQUIRE(s.has_failed());
  REQUIRE(s.get_fail() == "test");
}

TEST_CASE("state string") {
  std::string str = "hello";
  StateString s(&str);
  REQUIRE(s.adv() == 'h');
  REQUIRE(s.adv() == 'e');
  REQUIRE(s.adv() == 'l');
  REQUIRE(s.adv() == 'l');
  REQUIRE(s.adv() == 'o');
  REQUIRE_THROWS(s.adv());
}

TEST_CASE("state string stream") {
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

TEST_CASE("state file stream") {
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
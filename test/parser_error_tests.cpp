#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "parser.h"

void parseError(const std::string &source, const std::string &error)
{
  Package p;
  CHECK_THROWS_WITH(Parser(source, p).parse(), error);
}

void parseErrorThrow(const std::string &source, const std::string &)
{
  Package p;
  Parser(source, p).parse();
}

TEST_CASE("Parsing idls with error", "[]")
{
  parseError(R"(
table Dummy {
  a:int;)",
             "Missing closing '}'.");

  parseError(R"(
enum E {
  a
)",
             "Missing closing '}'.");

  parseError(R"(
package Scope
version "0.0";
)",
             "Expected ';' after package statement.");

  parseError(R"(
version "0.0"
package Scope;
)",
             "Expected ';' after version statement.");

  parseError(R"(
root_type Dummy
package Scope;
)",
             "Expected ';' after root_type statement.");

  parseError(R"(
table A {
  a:int
}
)",
             "Expected ';' after member definition.");

  parseError(R"(
table A {
  a:int
  b:int;
}
)",
             "Expected ';' after member definition.");

  parseError(R"(
table A {
  a:int = ;
}
)",
             "Missing default value.");

  parseError(R"(
table A {
  :int;
}
)",
             "Missing name for member definition.");

  parseError(R"(
table A {
  a:int =
  b:int;
}
  )",
             "Expected ';' after member definition.");

  parseError(R"(
package ;
)",
             "Expected package name after 'package'.");

  parseError(R"(
version ;
)",
             "Expected version string after 'version'.");

  parseError(R"(
root_type ;
)",
             "Expected table name after 'root_type'.");

  parseError(R"(
table {}
)",
             "Expected table name after 'table'.");

  parseError(R"(
enum {}
)",
             "Expected enum name after 'enum'.");
}

#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"
#include "parser.h"
#include "structurecheck.h"

void checkErrorInPure(const std::string &error, size_t line, size_t column, const std::string &source)
{
  Package p;
  REQUIRE(Parser(source, p).parse());

  const auto errors = StructureCheck(p).check();
  INFO(error);
  REQUIRE(errors.size() == 1);
  CHECK(errors.front().what() == error);
  CHECK(errors.front()._state.line == line);
  CHECK(errors.front()._state.column == column);
}

void addFooter(std::string &source)
{
  source +=
      "\n"
      "package Scope;\n"
      "version \"3.2.1\";\n"
      "root_type Dummy;\n"
      "table Dummy { x:int; }\n";
}

void checkErrorIn(const std::string &error, size_t line, size_t column, std::string source)
{
  addFooter(source);
  checkErrorInPure(error, line, column, source);
}

void checkNoErrorIn(std::string source)
{
  addFooter(source);

  Package p;
  REQUIRE(Parser(source, p).parse());

  const auto errors = StructureCheck(p).check();
  CAPTURE(errors);
  REQUIRE(errors.empty());
}

TEST_CASE("Check structure errors", "[error, parsing, structure]")
{
  SECTION("table structure errors")
  {
    checkErrorIn("Duplicate table 'Table1'.", 3, 1,
                 "table Table1 { c:int; }\n"
                 "table Table2 { c:int; }\n"
                 "table Table1 { a:int; }\n");
    checkErrorIn("Empty table 'Table2'.", 2, 2, "\n table Table2 {}");
    checkErrorIn("table member 'a' already defined for 'T1'.", 1, 24, "table T1 {a:int; b:i8; a:float;}");
    checkErrorIn("Unknown type 'xxx'.", 1, 11, "table T1 {a:xxx;}");
    checkErrorIn("Unknown type 'xxx'.", 2, 16,
                 "enum E{a,b,c}\n"
                 "table T1 {e:E; a:xxx;}");
  }

  SECTION("enum structure errors")
  {
    checkErrorIn("Duplicate enum 'E1'.", 3, 2,
                 "enum E1 { c }\n"
                 "enum E2 { c }\n"
                 " enum E1 { a }\n");
    checkErrorIn("enum 'E1' already defined as table.", 3, 2,
                 "table E1 { c:int; }\n"
                 "enum E2 { c }\n"
                 " enum E1 { a }\n");
    checkErrorIn("Empty enum 'E2'.", 1, 4, "   enum E2 {}");
    checkErrorIn("enum value 'a' already defined for 'E2'.", 1, 14, "enum E2 {a,b,a}");
  }

  SECTION("root_type errors")
  {
    string allText =
        "\n"
        "package Scope;\n"
        "version \"3.2.1\";\n"
        "table Dummy { x:int; }\n";

    checkErrorInPure("No root_type defined.", 1, 1, allText);
    checkErrorInPure("Unknown root_type 'Failure'.", 1, 1, "root_type Failure;" + allText);
    checkErrorInPure("root_type 'Failure' is an enum.", 1, 1,
                     "root_type Failure;\n"
                     "enum Failure { k }" +
                         allText);
  }

  SECTION("package errors")
  {
    string allText =
        "\n"
        "version \"3.2.1\";\n"
        "root_type Dummy;\n"
        "table Dummy { x:int; }\n";

    checkErrorInPure("No package defined.", 1, 1, allText);
  }

  SECTION("no pointer for base types")
  {
    checkErrorIn("base types cannot be pointer.", 2, 3,
                 "table T {\n"
                 "  c:shared int;\n"
                 "}\n");

    checkErrorIn("base types cannot be pointer.", 2, 3,
                 "table T {\n"
                 "  c:weak f32;\n"
                 "}\n");

    checkErrorIn("base types cannot be pointer.", 2, 3,
                 "table T {\n"
                 "  c:unique string;\n"
                 "}\n");
  }

  SECTION("no pointer for enums")
  {
    checkErrorIn("enums cannot be pointer.", 2, 3,
                 "table T {\n"
                 "  c:shared E;\n"
                 "}\n"
                 "enum E {a,b}\n");

    checkErrorIn("enums cannot be pointer.", 2, 3,
                 "table T {\n"
                 "  c:weak E;\n"
                 "}\n"
                 "enum E {a,b}\n");

    checkErrorIn("enums cannot be pointer.", 2, 3,
                 "table T {\n"
                 "  c:unique E;\n"
                 "}\n"
                 "enum E {a,b}\n");
  }

  SECTION("table special function checks")
  {
    SECTION("special initializer parsing")
    {
      checkErrorIn("unknown method '_fail_' in table 'T1'.", 3, 3,
                   "table T1 {\n"
                   "  a:int;\n"
                   "  _fail_(a);\n"
                   "}");

      checkErrorIn("No parameter defined for method 'init'.", 3, 3,
                   "table T1 {\n"
                   "  a:int;\n"
                   "  init();\n"
                   "}");

      checkErrorIn("unknown parameter 'b' in method 'init'.", 3, 11,
                   "table T1 {\n"
                   "  a:int;\n"
                   "  init(a, b);\n"
                   "}");

      checkErrorIn("duplicate parameter 'a' in method 'init'.", 3, 11,
                   "table T1 {\n"
                   "  a:int;\n"
                   "  init(a, a);\n"
                   "}");

      checkErrorIn("method 'init' already defined.", 4, 3,
                   "table T1 {\n"
                   "  a:int;\n"
                   "  init(a);\n"
                   "  init(a);\n"
                   "}");
      checkNoErrorIn(
          "table T1 {\n"
          "  a:int;\n"
          "  b:int;\n"
          "  init(a);\n"
          "  init(a,b);\n"
          "}");

      checkErrorIn("method 'init' already defined implicitly by parameter.", 5, 3,
                   "table T1 {\n"
                   "  a:int;\n"
                   "  b:int;\n"
                   "  init(a);\n"
                   "  init(b);\n"
                   "}");
    }
  }
}

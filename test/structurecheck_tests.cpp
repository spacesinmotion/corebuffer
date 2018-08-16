#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"
#include "parser.h"
#include "structurecheck.h"

void checkErrorInPure(const std::string &error, size_t line, size_t column, const std::string &source)
{
  INFO(error);

  Package p;
  REQUIRE_NOTHROW(Parser(source, p).parse());

  const auto errors = StructureCheck(p).check();
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
  REQUIRE_NOTHROW(Parser(source, p).parse());

  const auto errors = StructureCheck(p).check();
  CAPTURE(errors);
  REQUIRE(errors.empty());
}

TEST_CASE("Check structure errors", "[error, parsing, structure]")
{
  SECTION("table structure errors")
  {
    checkErrorIn("table 'Table1' already defined.", 3, 1,
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
    checkErrorIn("enum 'E1' already defined.", 3, 2,
                 "enum E1 { c }\n"
                 "enum E2 { c }\n"
                 " enum E1 { a }\n");
    checkErrorIn("enum 'E1' already defined.", 3, 2,
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

  SECTION("wrong union definitions")
  {
    checkErrorIn("union 'U1' already defined.", 3, 2,
                 "union U1 { Dummy }\n"
                 "union U2 { Dummy }\n"
                 " union U1 { Dummy }\n");
    checkErrorIn("union 'U1' already defined.", 3, 2,
                 "table U1 { c:int; }\n"
                 "union U2 { Dummy }\n"
                 " union U1 { Dummy }\n");
    checkErrorIn("union 'U1' already defined.", 3, 2,
                 "enum U1 { c }\n"
                 "union U2 { Dummy }\n"
                 " union U1 { Dummy }\n");
    checkErrorIn("Empty union 'U2'.", 1, 4, "   union U2 {}");
    checkErrorIn("union entry 'B' already defined for 'U2'.", 2, 19,
                 "table B{a:int;}\n"
                 "union U2 {B,Dummy,B}");
    checkErrorIn("Unknown table 'xxx'.", 1, 11, "union T1 {xxx, Dummy}");
  }

  SECTION("wrong default values")
  {
    SECTION("base types")
    {
      checkNoErrorIn("table T1 { a:float=42; }");
      checkNoErrorIn("table T1 { a:float=42.0; }");
      checkNoErrorIn("table T1 { a:int=42; }");
      checkNoErrorIn("table T1 { a:bool=true; }");
      checkNoErrorIn("table T1 { a:bool=false; }");
      checkNoErrorIn("table T1 { a:string=\"true/false\"; }");
      checkNoErrorIn("table T1 { a:string=\"\"; }");

      checkErrorIn("only integral values can be assigned here.", 2, 9,
                   "table T1 {\n"
                   "  a:int=0.4;\n"
                   "}");
      checkErrorIn("only integral values can be assigned here.", 2, 9,
                   "table T1 {\n"
                   "  a:int=\"0.4\";\n"
                   "}");
      checkErrorIn("only integral values can be assigned here.", 2, 9,
                   "table T1 {\n"
                   "  a:int=true;\n"
                   "}");
      checkErrorIn("only floating point values can be assigned here.", 2, 11,
                   "table T1 {\n"
                   "  a:float=\"0.4\";\n"
                   "}");
      checkErrorIn("only floating point values can be assigned here.", 2, 11,
                   "table T1 {\n"
                   "  a:float=false;\n"
                   "}");
      checkErrorIn("only string values can be assigned here.", 2, 12,
                   "table T1 {\n"
                   "  a:string=0.4;\n"
                   "}");
      checkErrorIn("only string values can be assigned here.", 2, 12,
                   "table T1 {\n"
                   "  a:string=false;\n"
                   "}");

      checkErrorIn("only boolean values can be assigned here.", 2, 10,
                   "table T1 {\n"
                   "  a:bool=1;\n"
                   "}");
      checkErrorIn("only boolean values can be assigned here.", 2, 10,
                   "table T1 {\n"
                   "  a:bool=4.3;\n"
                   "}");
      checkErrorIn("only boolean values can be assigned here.", 2, 10,
                   "table T1 {\n"
                   "  a:bool=\"fail\";\n"
                   "}");
    }

    SECTION("enums")
    {
      checkErrorIn("unknown value 'fail' for enum 'E1'.", 3, 8,
                   "enum E1 { A, B, C }\n"
                   "table T1 {\n"
                   "  a:E1=fail;\n"
                   "}");
      checkErrorIn("only values of 'E1' can be assigned here.", 3, 8,
                   "enum E1 { A, B, C }\n"
                   "table T1 {\n"
                   "  a:E1=true;\n"
                   "}");
    }

    SECTION("tables")
    {
      checkErrorIn("default values for tables are not supported.", 3, 8,
                   "table T1 { a:int; }\n"
                   "table T2 {\n"
                   "  a:T1=fail;\n"
                   "}");
    }

    SECTION("pointer")
    {
      checkErrorIn("default values for pointer are not supported.", 2, 15,
                   "table T2 {\n"
                   "  a:shared T2=fail;\n"
                   "}");
      checkErrorIn("default values for pointer are not supported.", 2, 13,
                   "table T2 {\n"
                   "  a:weak T2=fail;\n"
                   "}");
      checkErrorIn("default values for pointer are not supported.", 2, 15,
                   "table T2 {\n"
                   "  a:unique T2=fail;\n"
                   "}");
      checkErrorIn("default values for vectors are not supported.", 2, 11,
                   "table T2 {\n"
                   "  a:[int]=1;\n"
                   "}");
    }
  }
}

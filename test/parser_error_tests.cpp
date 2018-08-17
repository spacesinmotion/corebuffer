#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "fileerror.h"
#include "parser.h"

void checkThrowIn(const std::string &error, size_t line, size_t column, const std::string &source)
{
  Package p;
  try
  {
    Parser(source, p).parse();
    WARN("Never reach this Location");
    CHECK(false);
  }
  catch (const FileError &fe)
  {
    INFO(error);
    CHECK(fe.what() == error);
    CHECK(fe._state.line == line);
    CHECK(fe._state.column == column);
  }
}

TEST_CASE("Parsing idls with error", "[parsing, error, syntax]")
{
  checkThrowIn("Missing closing '}'.", 1, 14,
               "table Dummy {\n"
               "  a:int;\n");

  checkThrowIn("Missing closing '}'.", 1, 10,
               "table A {\n"
               "  :int;\n"
               "}");

  checkThrowIn("Missing closing '}'.", 1, 9,
               "enum E {\n"
               "  a\n");

  checkThrowIn("Expected ';' after package statement.", 1, 14,
               "package Scope\n"
               "version \"0.0\"");

  SECTION("version attribute")
  {
    checkThrowIn("Expected ';' after version statement.", 1, 14,
                 "version \"0.0\"\n"
                 "package Scope;");

    checkThrowIn("Expected version string after 'version'.", 1, 8, "version   ;");

    checkThrowIn("Line break in string constant.", 1, 10, "version  \"..as.;\n");
  }

  checkThrowIn("Expected ';' after root_type statement.", 1, 16,
               "root_type Dummy\n"
               "package Scope;");

  checkThrowIn("Expected ';' after member definition.", 2, 8,
               "table A {\n"
               "  a:int\n"
               "}");

  checkThrowIn("Expected ';' after member definition.", 3, 4,
               "table A {\n"
               "  a:int\n"
               "  b:int;\n"
               "}");

  checkThrowIn("Missing default value.", 2, 10,
               "table A {\n"
               "  a:int =  ;\n"
               "}\n");

  checkThrowIn("Expected ';' after member definition.", 3, 4,
               "table A {\n"
               "  a:int =\n"
               "  b:int;\n"
               "}");

  checkThrowIn("Expected package name after 'package'.", 1, 8, "package  ;");

  checkThrowIn("Expected table name after 'root_type'.", 1, 10, "root_type   ;");

  checkThrowIn("Expected table name after 'table'.", 1, 6, "table   {}");

  checkThrowIn("Expected enum name after 'enum'.", 1, 5, "enum  {}");

  checkThrowIn("Missing value for enumeration.", 2, 6,
               "enum Dummy {\n"
               "  a = ,\n"
               "}");

  checkThrowIn("Expected ':' and type definition after member.", 2, 4,
               "table A {\n"
               "  a;\n"
               "}");

  checkThrowIn("Expected type definition for member.", 2, 6,
               "table A {\n"
               "  a : ;\n"
               "}");

  checkThrowIn("Missing ']' for vector definition.", 2, 9,
               "table A {\n"
               "  a:[int;\n"
               "}");

  checkThrowIn("Missing type for vector definition.", 2, 6,
               "table A {\n"
               "  a:[ ];\n"
               "}");

  checkThrowIn("Missing type for unique member.", 2, 11,
               "table A {\n"
               "  a:unique   ;\n"
               "}");

  checkThrowIn("Missing type for weak member.", 2, 9,
               "table A {\n"
               "  a:weak  ;\n"
               "}");

  checkThrowIn("Missing type for shared member.", 2, 11,
               "table A {\n"
               "  a:shared;\n"
               "}");

  checkThrowIn("Missing type for plain member.", 2, 10,
               "table A {\n"
               "  a:plain;\n"
               "}");

  SECTION("special method parsing")
  {
    checkThrowIn("Missing closing ')' for method 'init'.", 3, 9,
                 "table T1 {\n"
                 "  a:int;\n"
                 "  init(a ;\n"
                 "}");
    checkThrowIn("Expected ';' after method definition.", 3, 10,
                 "table T1 {\n"
                 "  a:int;\n"
                 "  init(a)\n"
                 "}");
  }

  SECTION("default values")
  {
    checkThrowIn("Line break in string constant.", 2, 12,
                 "table T1 {\n"
                 "  a:string=\"..as.;\n"
                 "}");
    checkThrowIn("Missing closing '\"' reading string constant.", 2, 12,
                 "table T1 {\n"
                 "  a:string=\"..as.;}");
  }

  SECTION("union tables")
  {
    checkThrowIn("Expected union name after 'union'.", 1, 6, "union {}");
    checkThrowIn("Missing closing '}'.", 1, 14, "union Dummy { ,}");
  }
}

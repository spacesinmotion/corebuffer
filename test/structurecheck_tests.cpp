#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "parser.h"
#include "structurecheck.h"

void checkErrorIn(const std::string &error, size_t line, size_t column, const std::string &source)
{
  Package p;
  REQUIRE(Parser(source, p).parse());

  const auto errors = StructureCheck(p).check();
  REQUIRE(errors.size() == 1);
  CHECK(errors.front().what() == error);
  CHECK(errors.front()._state.line == line);
  CHECK(errors.front()._state.column == column);
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
  }

  SECTION("enum structure errors")
  {
    checkErrorIn("Duplicate enum 'E1'.", 3, 2,
                 "enum E1 { c }\n"
                 "enum E2 { c }\n"
                 " enum E1 { a }\n");

    checkErrorIn("Empty enum 'E2'.", 1, 4, "   enum E2 {}");
  }

  SECTION("root_type errors") {}
}

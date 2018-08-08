#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "tabletypes.h"

#include <sstream>

TEST_CASE("TableType test", "[]")
{
  SECTION("Compare operator")
  {
    CHECK(Scope::TableA() == Scope::TableA());
    CHECK_FALSE(Scope::TableA() != Scope::TableA());

    CHECK(Scope::TableB() == Scope::TableB());
    CHECK_FALSE(Scope::TableB() != Scope::TableB());

    CHECK(Scope::TableC() == Scope::TableC());
    CHECK_FALSE(Scope::TableC() != Scope::TableC());
  }

  SECTION("reading whats written")
  {
    Scope::TableB b;
    b.name = "TableB";
    Scope::TableC c;
    c.a.emplace_back();
    c.a.front().name = "TableA";
    c.b.emplace_back(b);

    Scope::TableC cIn;
    CHECK_FALSE(c == cIn);

    std::stringstream sOut;
    Scope::TableC_io().WriteTableC(sOut, c);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::TableC_io().ReadTableC(sIn, cIn);

    CHECK(c == cIn);
  }
}

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

  SECTION("reading whats written with shared data")
  {
    Scope::TableC c;
    c.a.emplace_back();
    c.a.front().name = "TableA";
    c.a.front().d1 = std::make_unique<Scope::TableD>();
    c.a.front().d1->name = "TableD_1";
    c.a.front().d3 = std::make_shared<Scope::TableD>();
    c.a.front().d3->name = "TableD_3";
    c.a.front().d2 = c.a.front().d3;
    c.a.front().d4 = c.a.front().d3;

    std::stringstream sOut;
    Scope::TableC_io().WriteTableC(sOut, c);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::TableC cIn;
    Scope::TableC_io().ReadTableC(sIn, cIn);

    REQUIRE(cIn.a.size() == 1);
    CHECK(cIn.a.front().name == "TableA");

    REQUIRE(cIn.a.front().d1);
    REQUIRE(cIn.a.front().d2.lock());
    REQUIRE(cIn.a.front().d3);
    REQUIRE(cIn.a.front().d4);

    CHECK(cIn.a.front().d1->name == "TableD_1");
    CHECK(cIn.a.front().d2.lock()->name == "TableD_3");
    CHECK(cIn.a.front().d3->name == "TableD_3");
    CHECK(cIn.a.front().d4->name == "TableD_3");

    CHECK(cIn.a.front().d2.lock() == cIn.a.front().d3);
    CHECK(cIn.a.front().d4 == cIn.a.front().d3);
  }

  SECTION("reading whats written with shared data and cross releation ship")
  {
    Scope::TableC c;
    c.a.emplace_back();
    c.a.front().name = "TableA";
    c.a.front().d3 = std::make_shared<Scope::TableD>();
    c.a.front().d3->name = "TableD_3";
    c.a.front().d3->a = std::make_shared<Scope::TableA>();
    c.a.front().d3->a->d2 = c.a.front().d3;
    c.a.front().d3->a->d3 = c.a.front().d3;
    c.a.front().d3->a->d4 = c.a.front().d3;

    std::stringstream sOut;
    Scope::TableC_io().WriteTableC(sOut, c);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::TableC cIn;
    Scope::TableC_io().ReadTableC(sIn, cIn);

    REQUIRE(cIn.a.size() == 1);
    CHECK(cIn.a.front().name == "TableA");

    REQUIRE(cIn.a.front().d3);
    CHECK(cIn.a.front().d3->name == "TableD_3");

    REQUIRE(cIn.a.front().d3->a);
    REQUIRE(cIn.a.front().d3->a->d2.lock());
    REQUIRE(cIn.a.front().d3->a->d3);
    REQUIRE(cIn.a.front().d3->a->d4);
    CHECK(cIn.a.front().d3->a->d2.lock() == cIn.a.front().d3);
    CHECK(cIn.a.front().d3->a->d3 == cIn.a.front().d3);
    CHECK(cIn.a.front().d3->a->d4 == cIn.a.front().d3);
  }
}

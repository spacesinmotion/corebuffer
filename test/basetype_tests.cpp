#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "basetypes.h"

#include <sstream>

TEST_CASE("BaseType test", "[]")
{
  SECTION("default values")
  {
    Scope::BaseTypes bt;

    CHECK(bt.a == 0);
    CHECK(bt.aa == 0);
    CHECK(bt.ab == 0);
    CHECK(bt.b == false);
    CHECK(bt.c == 0.0f);
    CHECK(bt.d == 0.0);
    CHECK(bt.e == 0);
    CHECK(bt.f == 0);
    CHECK(bt.g == 0);
    CHECK(bt.h == 0);
    CHECK(bt.i == 0u);
    CHECK(bt.j == 0u);
    CHECK(bt.k == 0u);
    CHECK(bt.l == 0u);
  }

  SECTION("Compare operator")
  {
    Scope::BaseTypes bt1;
    Scope::BaseTypes bt2;
    CHECK(bt1 == bt2);
    CHECK_FALSE(bt1 != bt2);
  }

  SECTION("reading whats written")
  {
    Scope::BaseTypes dOut;
    dOut.a = 1;
    dOut.aa = 2;
    dOut.ab = 3;
    dOut.b = true;
    dOut.c = 0.1f;
    dOut.d = 0.2f;
    dOut.e = 5;
    dOut.f = 6;
    dOut.g = 7;
    dOut.h = 8;
    dOut.i = 9;
    dOut.j = 10;
    dOut.k = 11;
    dOut.l = 12;

    Scope::BaseTypes dIn;
    CHECK_FALSE(dOut == dIn);

    std::stringstream sOut;
    Scope::BaseTypes_io().WriteBaseTypes(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::BaseTypes_io().ReadBaseTypes(sIn, dIn);

    CHECK(dOut == dIn);
  }
}
